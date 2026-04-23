#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "model/modelspace.h"
#include "core/colour.h"

typedef struct {
    int width;
    int height;
    int channels;
    unsigned char *data;
} Framebuffer;

//ScreenPosition is 0-indexed, i.e. runs from 0-[WIDTH-1]
typedef struct {
  int x;
  int y;
} ScreenPos;

Framebuffer framebuffer_create(int width, int height, int channels);
void framebuffer_destroy(Framebuffer *fb);
void framebuffer_clear(Framebuffer *fb, Colour colour);
void framebuffer_set_pixel(Framebuffer *fb, int x, int y, Colour colour);

#endif
