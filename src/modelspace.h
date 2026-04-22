#ifndef MODELSPACE_H
#define MODELSPACE_H

typedef struct {
  float x;
  float y;
  float z;
} Vertex;

//CubisRenderer has no support for normals or textures as Cubis does not use them
//Maybe in the future?
typedef struct {
  int vIndex1;
  int vIndex2;
  int vIndex3;
} Face;

#endif
