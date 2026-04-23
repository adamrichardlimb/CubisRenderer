#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "tga.h"
#include <assert.h>
#include "logging.h"
#include "render/raster.h"
#include "framebuffer/framebuffer.h"
#include "model/modelspace.h"
#include "core/colour.h"
#include "model/model.h"
#include "io/io.c"

#define WIDTH 1024
#define HEIGHT 1024

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

  Model obj = parse_obj_file();
  draw_model(&framebuffer, &obj);

  save_tga("output.tga", &image, TGA_RGB);
  return 0;
}
