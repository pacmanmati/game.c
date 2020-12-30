#pragma once

#include <vertex_data.h>
#include <face_data.h>

struct vertex_data * obj_parse_vertices(const char *);
struct face_data * obj_parse_faces(const char *);
static int obj_calc_final_face_element_amount(const char *);
static enum face_format obj_determine_face_format(const char *);
static int obj_get_num_delimited_entries_on_line(const char *);
static void obj_triangulate(int *, int, int **);
