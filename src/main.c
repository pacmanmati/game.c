#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <cglm/call.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <camera.h>
#include <window.h>
#include <shader.h>
#include <shader-program.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
const int FPS = 60;

const bool CAPPED_FPS = true;
const double MS_PER_UPDATE = 0.2;

bool running = true;

SDL_Window *window;

uint32_t EBO, VAO, VBO;

float xrot = 0.0f;
float yrot = 0.0f;

mat4 model;

uint32_t model_loc;
uint32_t view_loc;
uint32_t proj_loc;

const float fov = 90.0f, near = 0.1f, far = 100.0f;

camera *c;

const float ROT_SPEED = 15.0f;

vec3 up_dir = {0.0f, 1.0f, 0.0f};

vec3 cam_dir;
vec3 cam_dir_neg;
vec3 cam_dir_perp;
vec3 cam_dir_perp_neg;

const float MOUSE_SENSITIVITY = 0.1f;

const float MOVE_SPEED = 0.1f;

float yaw = -90.0f, pitch = 0.0f;

// textured + coloured cube
const float cube[] = {
  // near face
  // x , y     z         r     g     b    u     v
  -0.5f, 0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 0
  0.5f, 0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 1
  -0.5f, -0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 2
  0.5f, -0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 3
  // left face
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, -1.0f, 1.0f, // 4
  -0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, -1.0f, 0.0f, // 5
  // right face
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 2.0f, 1.0f, // 6
  0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 2.0f, 0.0f, // 7
  // top face
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 2.0f, // 8
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 2.0f, // 9
  // bottom face
  -0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, -1.0f, // 10
  0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, -1.0f, // 11
  // far face
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, -2.0f, // 12
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, -2.0f, // 13
  /* note: the far face is flipped relative to the rest of the cube.
     this is kind of hard to explain without a diagram, but the cube faces
     are all oriented like the near face in the uv unwrap, however, the
     back face is flipped when wrapping the cube. it is possible to fix this
     using negative uv wrapping (at the cost of two extra vertices) to flip
     the back texture but it's preferable to just uv map correctly in the first place. */
};

unsigned int indices[] = {
  // clockwise
  // near
  0, 1, 3,
  0, 3, 2,
  // left
  4, 0, 2,
  4, 2, 5,
  // right
  1, 6, 7,
  1, 7, 3,
  // top
  8, 9, 1,
  8, 1, 0,
  // bottom
  2, 3, 11,
  2, 11, 10,
  // far
  10, 11, 13,
  10, 13, 12,
};

vec3 cube_translates[] = {
  {0,0,0},
};

// variable timestep for rendering
void render(float delta_time)
{
  glmc_mat4_identity(model);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // gimbal lock might be interesting to implement
  glmc_rotate(model, glm_rad(xrot), (vec3){1.0f, 0.0f, 0.0f});
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  glmc_rotate(model, glm_rad(yrot), (vec3){0.0f, 1.0f, 0.0f});
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  // save the rotation
  mat4 save_model;
  glmc_mat4_copy(model, save_model);
  for (unsigned int i = 0; i < sizeof(indices) / sizeof(vec3); ++i) {
    glmc_translate(model, cube_translates[i]);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);
    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
    glmc_mat4_copy(save_model, model);
  }
  SDL_GL_SwapWindow(window);

}

// fixed timestep for e.g. physics
void update(float delta_time)
{

}

void mouse_camera(int32_t x, int32_t y)
{
  yaw += glm_rad((float) x * MOUSE_SENSITIVITY);
  pitch -= glm_rad((float) y * MOUSE_SENSITIVITY);
  
  float constraint = glm_rad(89.0f);
  if(pitch > constraint)
    pitch =  constraint;
  if(pitch < -constraint)
    pitch = -constraint;
  
  glmc_vec3_copy((vec3){cosf(yaw) * cosf(pitch), sinf(pitch), sinf(yaw) * cosf(pitch)}, cam_dir);
  glmc_vec3_normalize(cam_dir);
  glmc_vec3_negate_to(cam_dir, cam_dir_neg);
  // glmc_vec3_ortho(cam_dir, cam_dir_perp);
  glmc_vec3_copy(cam_dir, cam_dir_perp);
  // cam_dir_perp[1] = 0;
  glmc_vec3_rotate(cam_dir_perp, glm_rad(-90.0f), up_dir);
  cam_dir_perp[1] = 0;
  glmc_vec3_normalize(cam_dir_perp);
  glmc_vec3_negate_to(cam_dir_perp, cam_dir_perp_neg);
  camera_look_dir(c, cam_dir);
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, c->view_mat[0]);
}

void move_camera(camera *c, vec3 dir, float speed)
{
  glmc_vec3_scale(dir, speed, dir);
  camera_translate(c, dir);
  glmc_vec3_scale(dir, 1/speed, dir);
}

void wasd_camera(vec3 input_vel, float delta_time)
{
  if (input_vel[2] < 0) move_camera(c, cam_dir, MOVE_SPEED);
  if (input_vel[2] > 0) move_camera(c, cam_dir_neg, MOVE_SPEED);
  if (input_vel[0] < 0) move_camera(c, cam_dir_perp, MOVE_SPEED);
  if (input_vel[0] > 0) move_camera(c, cam_dir_perp_neg, MOVE_SPEED);
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, c->view_mat[0]);
}

void handle_input(SDL_Event event, float delta_time)
{
  vec3 input_vel = {0.0f, 0.0f, 0.0f};
  while (SDL_PollEvent(&event)) {
    // poll for events
    switch (event.type) {

    case SDL_QUIT:
      printf("quitting\n");
      running = false;
      break;

    case SDL_WINDOWEVENT:
      switch(event.window.event){
      case SDL_WINDOWEVENT_RESIZED:
	SCREEN_WIDTH = event.window.data1;
	SCREEN_HEIGHT = event.window.data2;
	float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	glmc_perspective_resize(aspect, c->proj_mat);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, c->proj_mat[0]);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      }
      break;

    case SDL_KEYDOWN:
      // check which key was pressed
      switch (event.key.keysym.sym) {
      case SDLK_q:
	running = false;
	break;
      case SDLK_DOWN:
	xrot -= ROT_SPEED;
	break;
      case SDLK_UP:
	xrot += ROT_SPEED;
	break;
      case SDLK_LEFT:
	yrot -= ROT_SPEED;
	break;
      case SDLK_RIGHT:
	yrot += ROT_SPEED;
	break;

      case SDLK_w:
	input_vel[2] = 1;
	wasd_camera(input_vel, delta_time);
	break;
      case SDLK_s:
	input_vel[2] = -1;
	wasd_camera(input_vel, delta_time);
	break;
      case SDLK_a:
	input_vel[0] = -1;
	wasd_camera(input_vel, delta_time);
	break;
      case SDLK_d:
	input_vel[0] = 1;
	wasd_camera(input_vel, delta_time);
	break;

      }
      break;

    case SDL_MOUSEMOTION:
      mouse_camera(event.motion.xrel, event.motion.yrel);
      break;

    }
  }
}

uint32_t shader_program_setup()
{
  bool err;
  uint32_t vertex, fragment, program;
  vertex = create_shader(GL_VERTEX_SHADER, "src/shaders/vertex.glsl");
  fragment = create_shader(GL_FRAGMENT_SHADER, "src/shaders/fragment.glsl");

  if (!(vertex && fragment)) {
    fputs("shader_program_setup(): one of the shaders is null.", stderr);
    exit(1);
  }

  program = create_shader_program();
  attach_shader(program, vertex);
  attach_shader(program, fragment);

  if ((err = link_program(program))) {
    fputs("shader_program_setup(): program link error", stderr);
    exit(1);
  }

  destroy_shader(vertex);
  destroy_shader(fragment);
  return program;
}

int main()
{

  window = create_window(SCREEN_HEIGHT, SCREEN_WIDTH);

  uint32_t program = shader_program_setup();
  glUseProgram(program);

  // TODO abstract into some sort of render concept
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // texture
  // TODO abstract texture concept
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(1);
  unsigned char *data = stbi_load("textures/512.png", &width, &height, &nrChannels, 0);
  if (!data) fputs("failed to load img.\n", stderr);
  uint32_t texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  // texture params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  // 3d stuff
  glmc_mat4_identity(model);

  c = create_camera((vec3){0.0f, 0.0f, 3.0f}, up_dir,
			    fov, SCREEN_HEIGHT, SCREEN_WIDTH, near, far);

  // this transformation order still confuses me
  // camera_translate(c, (vec3){0.0f, -1.0f, 0.0f});
  camera_look_at(c, (vec3){0.0f, 0.0f, 0.0f});
  // camera_translate(c, (vec3){3.0f, 0.0f, 0.0f});

  model_loc = glGetUniformLocation(program, "model");
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  view_loc = glGetUniformLocation(program, "view");
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, c->view_mat[0]);

  proj_loc = glGetUniformLocation(program, "proj");
  glUniformMatrix4fv(proj_loc, 1, GL_FALSE, c->proj_mat[0]);

  // bind the one we want draw - not necessary here
  /* glBindTexture(GL_TEXTURE_2D, texture); */

  // bind the one we want draw - not necessary here
  // glBindVertexArray(VAO);
  // wireframe mode
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  SDL_Event event;
  Uint64 current_frame, last_frame = SDL_GetPerformanceCounter();
  double elapsed_time, lag;

  while (running) {
    current_frame = SDL_GetPerformanceCounter();
    // elapsed time between frames in milliseconds
    elapsed_time = (double) (current_frame - last_frame) * 1000 / (double) SDL_GetPerformanceFrequency();
    last_frame = current_frame;
    lag += elapsed_time;

    if (CAPPED_FPS && elapsed_time < 1000 / FPS) {
      SDL_Delay((1000 / FPS) - elapsed_time);
      // std::cout << "skipping " << ((1000 / FPS) - elapsedTime) << "ms\n";
    }

    handle_input(event, elapsed_time);

    while (lag-- >= MS_PER_UPDATE) update(elapsed_time);

    render(elapsed_time);
    // std::cout << "elapsed time: " << elapsedTime << "\n";
    // std::cout << "framerate: " << 1000 / elapsedTime << "\n";
  }

  printf("%s\n", glGetString(GL_VERSION));
  destroy_camera(c);
  destroy_shader_program(program);
  destroy_window(window);

  return 0;
}
