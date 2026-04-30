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
  unsigned char *z_buffer = malloc(width*height);

  //We have to remove garbage data from the z_buffer as we will be testing it
  for (int fbIdx=0; fbIdx<width*height; fbIdx++) {
    z_buffer[fbIdx] = 0;
  }

  return (Framebuffer) {width, height, channels, data, z_buffer};
}

void framebuffer_destroy(Framebuffer *fb) {
  free(fb->data);
  free(fb->z_buffer);
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

  for (int fbIdx = 0; fbIdx < width*height; fbIdx++) {
    fb->z_buffer[fbIdx] = 0;
  }
}

int pixel_index(Framebuffer *fb, int x, int y) {
  const int width = fb->width;
  const int height = fb->height;

  const int pixelIdx = ((y * width) + x);
  assert(pixelIdx >= 0 && pixelIdx < width * height);
  return pixelIdx;  
}

void framebuffer_set_z_buffer(Framebuffer *fb, int x, int y, char buffer_value) {
  assert(fb);
  assert(fb->z_buffer);

  if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) {
    LOG("Attempted to set invalid z buffer pixel %d, %d. Not colouring and returning.", x, y);
    return;
  }

  int pixelIdx = pixel_index(fb, x, y);

  fb->z_buffer[pixelIdx] = buffer_value;
}

void framebuffer_set_pixel(Framebuffer *fb, int x, int y, Colour colour) {
  assert(fb);
  assert(fb->data);
  assert(fb->channels==3);

  int pixelIdx = pixel_index(fb, x, y);

  if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) {
    LOG("Attempted to set invalid pixel %d, %d. Not colouring and returning.", x, y);
    return;
  }

  //Multiply by channels to get index in buffer
  pixelIdx*=fb->channels;
  LOG("Colouring pixel at index %d", pixelIdx);

  fb->data[pixelIdx + 0] = colour.r;
  fb->data[pixelIdx + 1] = colour.g;
  fb->data[pixelIdx + 2] = colour.b;
}
