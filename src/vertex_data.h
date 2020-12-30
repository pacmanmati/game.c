#pragma once

struct vertex_data
{
  int length;
  float *data_v;
  float *data_vt;
  float *data_vn;
};

struct vertex_data * create_vertex_data(int);
void free_vertex_data(struct vertex_data *);
