#ifndef MODEL_H
#define MODEL_H

typedef struct {
  Vertex *vertices;
  int vertex_count;
  Face *faces;
  int face_count;
} Model;

#endif
