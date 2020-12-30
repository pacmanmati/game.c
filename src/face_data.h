#pragma once

#include <face_format.h>

struct face_data {
  int length; // all faces are turned into triangles
  enum face_format format; // v || v/vt || v/vt/vn || v//vn
  int *data_v;
  int *data_vt;
  int *data_vn;
};

struct face_data * create_face_data(int, enum face_format);
void free_face_data(struct face_data *);
