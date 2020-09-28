#include <GL/glew.h>
#include <GL/gl.h>

#include <window.h>

SDL_Window * create_window(int height, int width)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = window = SDL_CreateWindow
    ("OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
     width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  // use opengl 4.5 core - not sure if glew adjusts this number, doubt it can decrease it though
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // create context
  SDL_GLContext mainContext = SDL_GL_CreateContext(window);
  SDL_SetRelativeMouseMode(1);
  // initialise GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "GLEW %s works\n", glewGetString(GLEW_VERSION));
  return window;
}

void destroy_window(SDL_Window *window)
{
  SDL_DestroyWindow(window);
  SDL_Quit();
}
