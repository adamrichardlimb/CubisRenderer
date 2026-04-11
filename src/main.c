#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tga.h"
#include <assert.h>

#define WIDTH 64
#define HEIGHT 64

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

static inline bool two_points_equal(Point2D a, Point2D b) {
  return a.x == b.x && a.y == b.y;
}

static inline int pixel_index(Point2D point) {
  return ((point.y * WIDTH) + point.x)*3;
}

static inline void colour_pixel(unsigned char *framebuffer, int pixel, Colour colour) {
  assert(pixel >= 0 && pixel < WIDTH * HEIGHT * 3);

  framebuffer[pixel + 0] = colour.r;
  framebuffer[pixel + sizeof(char)] = colour.g;
  framebuffer[pixel + 2*sizeof(char)] = colour.b; 
}

//In this co-ordinate system - the top left is 0 and the bottom right is the height
//and we draw lines from the top on down
void draw_bresenham_line(unsigned char *framebuffer, Point2D starting_point, Point2D ending_point, Colour colour) {
  //If steep then transpose the image to make it shallow to avoid floating point issue issues
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
    int x = (int) (starting_point.x + (dx) * t);
    int y = (int) (starting_point.y + (dy) * t);
    Point2D pixel_to_colour = (Point2D) {x, y};

    if (steep) {
      pixel_to_colour = (Point2D) {y, x};
    }

    colour_pixel(framebuffer, pixel_index(pixel_to_colour), colour);
    printf("Draw Pixel at: (%d, %d)\n", x, y); 
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

  save_tga("output.tga", &image, TGA_RGB);
  return 0;
}
