#pragma once

#include <cglm/call.h>

typedef struct
{
  mat4 view_mat, proj_mat;
  vec3 pos;
  vec3 up;
} camera;

camera * create_camera(vec3 pos, vec3 up, float fov, int w_height, int w_width, float near, float far);
void destroy_camera(camera *c);
void camera_window_resize(camera *c, float height, float width);
void camera_translate(camera *c, vec3 translation);
void camera_look_at(camera *c, vec3 target);
void camera_look_dir(camera *c, vec3 dir);
