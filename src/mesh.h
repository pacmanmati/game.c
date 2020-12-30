#pragma once

#include <vertex_data.h>
#include <face_data.h>

typedef struct
{
  struct vertex_data *vertices;
  struct face_data *faces;
  // unsigned int draw_mode;
} mesh;

mesh * mesh_create(const char *path);
void mesh_draw();
