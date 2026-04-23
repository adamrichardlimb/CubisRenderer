#include <stdio.h>
#include <stdlib.h>
#include "model/modelspace.h"
#include "model/model.h"
#include "logging.h"

static Model parse_obj_file() {
  //Read in file
  FILE *fptr;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  //Read in file line-by-line
  fptr = fopen("diablo3_post.obj", "r");

  if (fptr == NULL) {
    printf("Unable to open diablo3_post.obj, closing file!\n");
    exit(EXIT_FAILURE);
  }

  int line_count = 0;

  Vertex *vertices = NULL;
  Face *faces = NULL;

  int vertex_count = 0;
  int vertex_capacity = 0;

  int face_count = 0;
  int face_capacity = 0;

  while ((read = getline(&line, &len, fptr)) != -1) {
    line_count += 1;
    float x, y, z;

    //Read in each vertex and stick into the array increasing in size if we exceed our limit
    if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
      //It too large - reallocate
      if (vertex_count >= vertex_capacity) {
        vertex_capacity = vertex_capacity ? vertex_capacity * 2 : 128;
        vertices = realloc(vertices, vertex_capacity * sizeof(Vertex));
      }
      vertices[vertex_count++] = (Vertex){x, y, z};

      LOG("Vertex: %f %f %f", x, y, z);
    }

    int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
    //For faces read in the line and extract only the first number in each vertex definition
    if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9) {
      //It too large - reallocate
      if (face_count >= face_capacity) {
        face_capacity = face_capacity ? face_capacity * 2 : 128;
        faces = realloc(faces, face_capacity * sizeof(Face));
      }
      faces[face_count++] = (Face){v1, v2, v3};

      LOG("Face: %d %d %d", v1, v2, v3);
    }
  }

  fclose(fptr);
  if (line) {
    free(line);
  }

  return (Model) {vertices, vertex_count, faces, face_count};
}
