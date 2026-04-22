//ScreenPosition is 0-indexed, i.e. runs from 0-[WIDTH-1]
typedef struct {
  int x, y;
} ScreenPosition;

//Project takes some vertices in -1 <-> 1 in x/y and projects them into ScreenPosition
void project(Vertex *vertex) {
  //TODO - Drop vertices which have z <= 0 (i.e. behind camera)
  //Then we do xPrime and yPrime by x/z and y/z
  return (ScreenPosition) {(int) (vertex->x+1.0f) * (WIDTH - 1)/2.0f, (int) (vertex->y+1.0f) * (HEIGHT - 1)/2.0f};
}
