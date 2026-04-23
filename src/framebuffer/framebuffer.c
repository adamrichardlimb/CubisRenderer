#include "framebuffer/framebuffer.h"
#include "render/raster.h"
#include "model/modelspace.h"
#include "logging.h"

#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

Framebuffer framebuffer_create(int width, int height, int channels) {
  unsigned char *data = malloc(width*height*channels);
  assert(data != NULL);
  return (Framebuffer) {width, height, channels, data};
}

void framebuffer_destroy(Framebuffer *fb) {
  free(fb->data);
  fb->data = NULL;
  fb->width = 0;
  fb->height = 0;
  fb->channels = 0;
}

//TODO - Add support for multi-channel or move to RGBA - currently only support RGB
//CONSIDER - Shouldn't there be some kind of windowed assembly instruction which just dumps the values of a window into some space in memory?
//That would be cool and give me mad nerd kudos
void framebuffer_clear(Framebuffer *fb, Colour colour) {
  int width = fb->width;
  int height = fb->height;
  int channels = fb->channels;

  //Iterate over each pixel of the framebuffer
  for (int fbIdx = 0; fbIdx < width*height*channels; fbIdx+=channels) {
    fb->data[fbIdx + 0] = colour.r;
    fb->data[fbIdx + 1] = colour.g;
    fb->data[fbIdx + 2] = colour.b; 
  }
}

void framebuffer_set_pixel(Framebuffer *fb, int x, int y, Colour colour) {
  assert(fb);
  assert(fb->data);
  assert(fb->channels==3);

  const int width = fb->width;
  const int height = fb->height;
  const int channels = fb->channels;

  if (x < 0 || x >= width || y < 0 || y >= height) {
    LOG("Attempted to set invalid pixel %d, %d. Not colouring and returning.", x, y);
    return;
  }
 
  //((point.y * WIDTH) + point.x)*3
  const int pixelIdx = ((y * width) + x)*channels;
  assert(pixelIdx >= 0 && pixelIdx < width * height * channels);

  LOG("Colouring pixel at index %d", pixelIdx);

  fb->data[pixelIdx + 0] = colour.r;
  fb->data[pixelIdx + 1] = colour.g;
  fb->data[pixelIdx + 2] = colour.b;
}
