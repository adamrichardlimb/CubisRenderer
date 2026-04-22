#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "tga.h"
#include <assert.h>
#include "logging.h"
#include "colour.h"
#include "framebuffer.h"
#include "modelspace.h"

#define WIDTH 1024
#define HEIGHT 1024

typedef struct {
  Vertex *vertices;
  int vertex_count;
  Face *faces;
  int face_count;
} wireframe_obj;

static wireframe_obj parse_obj_file() {
  //Read in file
  FILE *fptr;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  //Read in file line-by-line
  fptr = fopen("diablo3_post.obj", "r");

  if (fptr == NULL) {
    printf("Unable to open diablo3_post.obj, closing file!\n");
    exit(EXIT_FAILURE);
  }

  int line_count = 0;

  Vertex *vertices = NULL;
  Face *faces = NULL;

  int vertex_count = 0;
  int vertex_capacity = 0;

  int face_count = 0;
  int face_capacity = 0;

  while ((read = getline(&line, &len, fptr)) != -1) {
    line_count += 1;
    float x, y, z;

    //Read in each vertex and stick into the array increasing in size if we exceed our limit
    if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
      //It too large - reallocate
      if (vertex_count >= vertex_capacity) {
        vertex_capacity = vertex_capacity ? vertex_capacity * 2 : 128;
        vertices = realloc(vertices, vertex_capacity * sizeof(Vertex));
      }
      vertices[vertex_count++] = (Vertex){x, y, z};

      LOG("Vertex: %f %f %f", x, y, z);
    }

    int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
    //For faces read in the line and extract only the first number in each vertex definition
    if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9) {
      //It too large - reallocate
      if (face_count >= face_capacity) {
        face_capacity = face_capacity ? face_capacity * 2 : 128;
        faces = realloc(faces, face_capacity * sizeof(Face));
      }
      faces[face_count++] = (Face){v1, v2, v3};

      LOG("Face: %d %d %d", v1, v2, v3);
    }
  }

  fclose(fptr);
  if (line) {
    free(line);
  }

  return (wireframe_obj) {vertices, vertex_count, faces, face_count};
}

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

void draw_obj(Framebuffer *framebuffer, wireframe_obj *obj) {
  //Iterate over the faces and access the members of the vertices array by their index
  for (int faceIdx = 0; faceIdx < obj->face_count; faceIdx++) {
    Face face = obj->faces[faceIdx];

    Vertex vertex1 = obj->vertices[face.vIndex1 - 1];
    Vertex vertex2 = obj->vertices[face.vIndex2 - 1];
    Vertex vertex3 = obj->vertices[face.vIndex3 - 1];

    //Scale to our image
    ScreenPos point1 = project_to_framebuffer(framebuffer, &vertex1);
    ScreenPos point2 = project_to_framebuffer(framebuffer, &vertex2);
    ScreenPos point3 = project_to_framebuffer(framebuffer, &vertex3);

    draw_bresenham_line(framebuffer, point1, point2, COLOUR_RED);
    draw_bresenham_line(framebuffer, point2, point3, COLOUR_RED);
    draw_bresenham_line(framebuffer, point3, point1, COLOUR_RED);
  }
}


int main(void) {
  //One byte for rgb
  Framebuffer framebuffer = framebuffer_create(WIDTH, HEIGHT, 3);

  tga_image image = (tga_image) {
    WIDTH,
    HEIGHT,
    3,
    framebuffer.data,
    false,
    false
  };

  wireframe_obj obj = parse_obj_file();
  draw_obj(&framebuffer, &obj);

  save_tga("output.tga", &image, TGA_RGB);
  return 0;
}
