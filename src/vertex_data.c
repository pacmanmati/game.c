#include <vertex_data.h>
#include <face_format.h>
#include <stdlib.h>

struct vertex_data * create_vertex_data(int length)
{
  struct vertex_data *v = malloc(sizeof(struct vertex_data));
  v->length = length;
  v->data_v = malloc(sizeof(float) * length);
  v->data_vt = NULL;
  v->data_vn = NULL;
}

void free_vertex_data(struct vertex_data *v)
{
  free(v->data_v);
  free(v->data_vt);
  free(v->data_vn);
  free(v);
}
