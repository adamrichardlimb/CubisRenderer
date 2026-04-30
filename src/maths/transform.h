#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "framebuffer/framebuffer.h"
#include "logging.h"

//Project takes some vertices in -1 <-> 1 in x/y and projects them into ScreenProjection
ScreenProjection project_to_screen(Framebuffer *fb, Vertex *vertex) {
  //TODO - Drop vertices which have z <= 0 (i.e. behind camera)
  //Then we do xPrime and yPrime by x/z and y/z
  return (ScreenProjection) {(vertex->x+1.0f) * (fb->width - 1)/2.0f, (vertex->y+1.0f) * (fb->height - 1)/2.0f, (vertex->z+1.0f)*255.0f/2.0f};
}

#endif
