#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "tga.h"
#include <assert.h>
#include "logging.h"

#define WIDTH 1024
#define HEIGHT 1024

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Colour;

#define WHITE ((Colour){255, 255, 255})
#define RED ((Colour){255, 0, 0})
#define GREEN ((Colour){0, 255, 0})
#define BLUE ((Colour){0, 0, 255})
#define YELLOW ((Colour){250, 210, 10})
#define PURPLE ((Colour){250, 10, 250})
#define CYAN ((Colour){10, 250, 250})
#define BLACK ((Colour){0, 0, 0})

typedef struct {
  int x;
  int y;
} Point2D;

Point2D a1 = (Point2D) {7, 3};
Point2D a2 = (Point2D) {12, 37};
Point2D a3 = (Point2D) {62, 53};

typedef struct {
  float x;
  float y;
  float z;
} Vertex;

typedef struct {
  int vIndex1;
  int vIndex2;
  int vIndex3;
} ObjFace;

typedef struct {
  Vertex *vertices;
  int *faces;
} wireframe_obj;

static void parse_obj_file() {
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
  ObjFace *faces = NULL;

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
        faces = realloc(faces, face_capacity * sizeof(ObjFace));
      }
      faces[face_count++] = (ObjFace){v1, v2, v3};

      LOG("Face: %d %d %d", v1, v2, v3);
    }
  }

  fclose(fptr);
  if (line) {
    free(line);
  }
}

static inline bool two_points_equal(Point2D a, Point2D b) {
  return a.x == b.x && a.y == b.y;
}

static inline int pixel_index(Point2D point) {
  return ((point.y * WIDTH) + point.x)*3;
}

static inline void colour_pixel(unsigned char *framebuffer, int pixel, Colour colour) {
  assert(pixel >= 0 && pixel < WIDTH * HEIGHT * 3);

  framebuffer[pixel + 0] = colour.r;
  framebuffer[pixel + 1] = colour.g;
  framebuffer[pixel + 2] = colour.b; 
}

//In this co-ordinate system - the top left is 0 and the bottom right is the height
//and we draw lines from the top on down
void draw_bresenham_line(unsigned char *framebuffer, Point2D starting_point, Point2D ending_point, Colour colour) {
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
    Point2D temp = starting_point;
    starting_point = ending_point;
    ending_point = temp;
  }

  int dx = ending_point.x - starting_point.x;
  int dy = ending_point.y - starting_point.y;

  //Main Bresenham algorithm
  for (float x = starting_point.x; x < ending_point.x; x++) {
    float t = (float) ((x-starting_point.x)/(ending_point.x - starting_point.x));
    int y = round(starting_point.y + (dy) * t);
    Point2D pixel_to_colour = (Point2D) {x, y};

    if (steep) {
      pixel_to_colour = (Point2D) {y, x};
    }

    colour_pixel(framebuffer, pixel_index(pixel_to_colour), colour);
    LOG("Draw Pixel at: (%d, %d) with sample %f", pixel_to_colour.x, pixel_to_colour.y, t); 
  }
}

int main(void) {
  //One byte for rgb
  unsigned char framebuffer[WIDTH*HEIGHT*3];

  for (int idx = 0; idx < WIDTH*HEIGHT*3; idx++) {
    framebuffer[idx] = 0;
  }

  draw_bresenham_line(framebuffer, a1, a2, RED);
  draw_bresenham_line(framebuffer, a2, a1, YELLOW);

  draw_bresenham_line(framebuffer, a3, a2, BLUE);
  draw_bresenham_line(framebuffer, a2, a3, PURPLE);

  draw_bresenham_line(framebuffer, a1, a3, GREEN);
  draw_bresenham_line(framebuffer, a3, a1, CYAN);

  colour_pixel(framebuffer, pixel_index(a1), WHITE);
  colour_pixel(framebuffer, pixel_index(a2), WHITE);
  colour_pixel(framebuffer, pixel_index(a3), WHITE);

  tga_image image = (tga_image) {
    WIDTH,
    HEIGHT,
    3,
    framebuffer,
    false,
    false
  };

  parse_obj_file();
  save_tga("output.tga", &image, TGA_RGB);
  return 0;
}
