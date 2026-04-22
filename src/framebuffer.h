#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "modelspace.h"
#include "colour.h"

typedef struct {
    int width;
    int height;
    int channels;
    unsigned char *data;
} Framebuffer;

typedef struct {
  int x;
  int y;
} ScreenPos;

Framebuffer framebuffer_create(int width, int height, int channels);
void framebuffer_destroy(Framebuffer *fb);
void framebuffer_clear(Framebuffer *fb, Colour colour);
void framebuffer_set_pixel(Framebuffer *fb, int x, int y, Colour colour);

ScreenPos project_to_framebuffer(Framebuffer *fb, Vertex *vertex);

#endif
