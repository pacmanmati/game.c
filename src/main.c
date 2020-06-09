#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <cglm/call.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <window.h>
#include <shader.h>
#include <shader-program.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FPS = 60;

const bool CAPPED_FPS = true;
const double MS_PER_UPDATE = 0.2;

bool running = true;

SDL_Window *window;

uint32_t EBO, VAO, VBO;

float xrot = 0.0f;
float yrot = 0.0f;

uint32_t model_loc;
mat4 model;

// textured + coloured cube
const float cube[] = {
  // near face
  // x,  y,    z,       r,    g,    b,    u,    v,
  -0.5f, 0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 0
  0.5f, 0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 1
  -0.5f, -0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 2
  0.5f, -0.5f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 3
  // left face
  /* -0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, */
  /* -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, */
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, -1.0f, 1.0f, // 4
  -0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, -1.0f, 0.0f, // 5
  // right face
  /* 0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, */
  /* 0.5f, -0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, */
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 2.0f, 1.0f, // 6
  0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 2.0f, 0.0f, // 7
  // top face
  /* -0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, */
  /* 0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, */
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, 2.0f, // 8
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, 2.0f, // 9
  // bottom face
  /* -0.5f, -0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, */
  /* 0.5f, -0.5f, 0.5f,	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, */
  -0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, -1.0f, // 10
  0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, -1.0f, // 11

  // far face
  /* -0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f, 0.0f, -1.0f, */
  /* 0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 0.0f, 1.0f, -1.0f, */
  -0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 0.0f, -2.0f, // 12
  0.5f, 0.5f, -0.5f,	1.0f, 1.0f, 1.0f, 1.0f, -2.0f, // 13
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
  {0,1,0},
  {0,-1,0},

  {0,0,-2},
  {0,1,-2},
  {0,-1,-2},
};

// variable timestep for rendering
void render()
{
  glmc_mat4_identity(model);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // gimbal lock might be interesting to implement
  glmc_rotate(model, xrot, (vec3){1.0f, 0.0f, 0.0f});
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  glmc_rotate(model, yrot, (vec3){0.0f, 1.0f, 0.0f});
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  // save the rotation
  mat4 save_model;
  glmc_mat4_copy(model, save_model);
  for (int i = 0; i < sizeof(indices) / sizeof(vec3); ++i) {
    glmc_translate(model, cube_translates[i]);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);
    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
    glmc_mat4_copy(save_model, model);
  }  
  SDL_GL_SwapWindow(window);
  
}

// fixed timestep for e.g. physics
void update()
{

}

void handle_input(SDL_Event event)
{
  while (SDL_PollEvent(&event)) {
    // poll for events
    switch (event.type) {
    case SDL_QUIT:
      printf("quitting\n");
      running = false;
      break;
    case SDL_KEYDOWN:
      // check which key was pressed
      switch (event.key.keysym.sym) {
      case SDLK_q:
	running = false;
	break;
      case SDLK_DOWN:
	xrot -= 0.1f;
	break;
      case SDLK_UP:
	xrot += 0.1f;
	break;
      case SDLK_LEFT:
	yrot -= 0.1f;
	break;
      case SDLK_RIGHT:
	yrot += 0.1f;
	break;
      }
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
  unsigned char *data = stbi_load("textures/brick.jpg", &width, &height, &nrChannels, 0);
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
  /* glmc_rotate(model, 5, (vec3){1.0f, 0.0f, 0.0f}); */
  
  mat4 view;
  glmc_mat4_identity(view);
  // camera transform axes are reversed
  glmc_translate(view, (vec3){0.0f, 0.0f, -3.0f});

  float fov, aspect, near, far;
  fov = 90.0f;
  aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
  near = 0.01f;
  far = 1000.0f;
  mat4 proj;
  glmc_perspective(fov, aspect, near, far, proj);

  model_loc = glGetUniformLocation(program, "model");
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);

  uint32_t view_loc = glGetUniformLocation(program, "view");
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, view[0]);

  uint32_t proj_loc = glGetUniformLocation(program, "proj");
  glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj[0]);

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
  Uint64 currentFrame, lastFrame = SDL_GetPerformanceCounter();
  double elapsedTime, lag;

  while (running) {
    currentFrame = SDL_GetPerformanceCounter();
    // elapsed time between frames in milliseconds
    elapsedTime = (double) (currentFrame - lastFrame) * 1000 / (double) SDL_GetPerformanceFrequency();
    lastFrame = currentFrame;
    lag += elapsedTime;

    if (CAPPED_FPS && elapsedTime < 1000 / FPS) {
      SDL_Delay((1000 / FPS) - elapsedTime);
      // std::cout << "skipping " << ((1000 / FPS) - elapsedTime) << "ms\n";
    }

    handle_input(event);

    while (lag-- >= MS_PER_UPDATE) update();
    
    render();
    // std::cout << "elapsed time: " << elapsedTime << "\n";
    // std::cout << "framerate: " << 1000 / elapsedTime << "\n";
  }

  printf("%s\n", glGetString(GL_VERSION));
  destroy_shader_program(program);
  destroy_window(window);

  return 0;
}
