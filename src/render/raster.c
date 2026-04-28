#include "render/raster.h"
#include "core/colour.h"
#include "framebuffer/framebuffer.h"
#include "logging.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "model/model.h"
#include "maths/transform.h"
#include "maths/maths.h"
#include "time.h"

//In this co-ordinate system - the top left is 0 and the bottom right is the height
//and we draw lines from the top on down
void draw_bresenham_line(Framebuffer *framebuffer, ScreenPos starting_point, ScreenPos ending_point, Colour colour) {
  //Bresnham works by having the movement in x always be +1 and in y be +1 or 0
  //This is true only when x increases faster than y
  //So in the event y increases faster then x - we must swap
  bool steep = abs(starting_point.x - ending_point.x) < abs(starting_point.y - ending_point.y);
  if (steep) {
    int temp = starting_point.x;
    starting_point.x = starting_point.y;
    starting_point.y = temp;

    temp = ending_point.x;
    ending_point.x = ending_point.y;
    ending_point.y = temp;
  }

  if (starting_point.x > ending_point.x) {
    ScreenPos temp = starting_point;
    starting_point = ending_point;
    ending_point = temp;
  }

  int dx = ending_point.x - starting_point.x;
  int dy = ending_point.y - starting_point.y;

  //Main Bresenham algorithm
  for (float x = starting_point.x; x < ending_point.x; x++) {
    float t = (float) ((x-starting_point.x)/(ending_point.x - starting_point.x));
    int y = round(starting_point.y + (dy) * t);
    ScreenPos pixel_to_colour = (ScreenPos) {x, y};

    if (steep) {
      pixel_to_colour = (ScreenPos) {y, x};
    }

    framebuffer_set_pixel(framebuffer, pixel_to_colour.x, pixel_to_colour.y, colour);
    LOG("Draw Pixel at: (%d, %d) with sample %f", pixel_to_colour.x, pixel_to_colour.y, t); 
  }
}

BoundingBox find_triangle_bounding_box(ScreenPos a, ScreenPos b, ScreenPos c) {
  int minX = a.x;
  minX = min(minX, b.x);
  minX = min(minX, c.x);

  int minY = a.y;
  minY = min(minY, b.y);
  minY = min(minY, c.y);

  int maxX = a.x;
  maxX = max(maxX, b.x);
  maxX = max(maxX, c.x);

  int maxY = a.y;
  maxY = max(maxY, b.y);
  maxY = max(maxY, c.y);

  return (BoundingBox) {(ScreenPos) {minX, minY}, (ScreenPos) {maxX, maxY}};
}

double signed_triangle_area(ScreenPos a, ScreenPos b, ScreenPos c) {
  return 0.5 * ((b.y-a.y)*(b.x+a.x) + (c.y-b.y)*(c.x+b.x) + (a.y-c.y)*(a.x+c.x));
}

void draw_filled_triangle(Framebuffer *framebuffer, ScreenPos a, ScreenPos b, ScreenPos c, Colour colour) {
  //Get bounding box
  BoundingBox bbox = find_triangle_bounding_box(a, b, c);

  double total_area = signed_triangle_area(a, b, c);

  //Go over every pixel - only colour if point in Baryocentric co-ordinates is within triangle
  //This is done by checking if each term Baryocentrically is positive
  #pragma omp parrallel for
  for (int x = bbox.pointA.x; x <= bbox.pointB.x; x++) {
    for (int y = bbox.pointA.y; y <= bbox.pointB.y; y++) {
      ScreenPos current_pixel = (ScreenPos) {x, y};
      double alpha = signed_triangle_area(current_pixel, b, c) / total_area;
      double beta = signed_triangle_area(current_pixel, c, a) / total_area;
      double gamma = signed_triangle_area(current_pixel, a, b) / total_area;

      //If not in triangle - ignore
      if (alpha<0||beta<0||gamma<0) continue;

      framebuffer_set_pixel(framebuffer, x, y, colour);
    }
  }
}

void draw_wireframe_triangle(Framebuffer *framebuffer, ScreenPos a, ScreenPos b, ScreenPos c, Colour colour) {
  draw_bresenham_line(framebuffer, a, b, colour);
  draw_bresenham_line(framebuffer, b, c, colour);
  draw_bresenham_line(framebuffer, c, a, colour);
}

void draw_model(Framebuffer *framebuffer, Model *model) {
  //Iterate over the faces and access the members of the vertices array by their index
  for (int faceIdx = 0; faceIdx < model->face_count; faceIdx++) {
    Face face = model->faces[faceIdx];

    Vertex vertex1 = model->vertices[face.vIndex1 - 1];
    Vertex vertex2 = model->vertices[face.vIndex2 - 1];
    Vertex vertex3 = model->vertices[face.vIndex3 - 1];

    //Scale to our image
    ScreenPos point1 = project_to_screen(framebuffer, &vertex1);
    ScreenPos point2 = project_to_screen(framebuffer, &vertex2);
    ScreenPos point3 = project_to_screen(framebuffer, &vertex3);
    srand(time(NULL));
    Colour test_colour = (Colour) {rand() % 255, rand() % 255, rand() % 255};
    draw_filled_triangle(framebuffer, point1, point2, point3, test_colour);
  }
}
