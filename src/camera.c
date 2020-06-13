#include <stdio.h>
#include <stdio.h>

#include <cglm/call.h>
#include <camera.h>

camera * create_camera(vec3 pos, vec3 up, float fov, int w_height, int w_width, float near, float far)
{
  camera *c = malloc(sizeof(camera));
  glmc_vec3_copy(pos, c->pos);
  glmc_vec3_copy(up, c->up);
  glmc_mat4_identity(c->view_mat);
  if (pos) glmc_translate(c->view_mat, pos);
  glmc_perspective(glm_rad(fov), (float) w_width / (float) w_height, near, far, c->proj_mat);
  return c;
}

void destroy_camera(camera *c)
{
  free(c);
}

// make the camera adjust its projection matrix using new window size
void camera_window_resize(camera *c, float height, float width)
{
  glmc_perspective_resize((float) width / (float) height, c->proj_mat);
}

void camera_translate(camera *c, vec3 translation)
{
  glmc_translate(c->view_mat, translation);
  glmc_vec3_negate(translation);
  glmc_vec3_add(translation, c->pos, c->pos);
  glmc_vec3_negate(translation);
}

void camera_look_at(camera *c, vec3 target)
{
  glmc_lookat(c->pos, target, c->up, c->view_mat);
}

void camera_look_dir(camera *c, vec3 dir)
{
  glmc_look(c->pos, dir, c->up, c->view_mat);
}
