#include "render/raster.h"
#include "core/colour.h"
#include "framebuffer/framebuffer.h"
#include "logging.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "model/model.h"
#include "maths/transform.h"

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

    draw_bresenham_line(framebuffer, point1, point2, COLOUR_RED);
    draw_bresenham_line(framebuffer, point2, point3, COLOUR_RED);
    draw_bresenham_line(framebuffer, point3, point1, COLOUR_RED);
  }
}
