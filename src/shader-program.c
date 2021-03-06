#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <shader-program.h>

uint32_t create_shader_program()
{
  return glCreateProgram();
}

void attach_shader(const uint32_t program, const uint32_t shader)
{
  glAttachShader(program, shader);
}

bool link_program(const uint32_t program)
{
  glLinkProgram(program);
  bool linked;
  glGetProgramiv(program, GL_LINK_STATUS, (int *) &linked);
  if (linked)
    return 0; // success
  else {
    int max_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);
    char *log = malloc(max_len * sizeof(char));
    if (log) {
      glGetProgramInfoLog(program, max_len, &max_len, log);
      fprintf(stderr, "link_program(): shader-program linking error, log:\n%s\n", log);
    } else {
      fputs("link_program(): malloc null ptr.\n", stderr);
    }
    return -1; // err
  }
}

void destroy_shader_program(const uint32_t program)
{
  glDeleteProgram(program);
}
