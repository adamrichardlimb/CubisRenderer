#ifndef RASTER_H
#define RASTER_H

#include "core/colour.h"
#include "framebuffer/framebuffer.h"
#include "model/model.h"

void draw_bresenham_line(Framebuffer *framebuffer, ScreenPos starting_point, ScreenPos ending_point, Colour colour);
void draw_model(Framebuffer *framebuffer, Model *model);

typedef struct {
  float weight;
  ScreenPos position;
} BaryocentricTerm;

typedef struct {
  BaryocentricTerm a;
  BaryocentricTerm b;
  BaryocentricTerm c;
} BaryocentricTriangle;

#endif
