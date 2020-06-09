#include <GL/glew.h>

#include <io.h>
#include <shader.h>

uint32_t create_shader(const uint32_t type, const char *path)
{
  uint32_t shader = glCreateShader(type);
  const char *source = read_file(path);
  if (source) {
    glShaderSource(shader, 1, &source, NULL);
    free((char *)source); // glShaderSource copies the string so we can free immediately
    glCompileShader(shader);

    int compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled) {
      return shader;
    } else {
      int max_len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len); // includes \0
      char *log = malloc(max_len * sizeof(char));
      if (log) {
	glGetShaderInfoLog(shader, max_len, &max_len, log);
	fprintf(stderr, "create_shader(): shader compilation error, log:\n%s\n", log);
      } else fprintf(stderr, "create_shader(): malloc null ptr.\n");
    }
  }
  // gl reserves 'zero' object id as a sort of NULL
  // https://www.khronos.org/opengl/wiki/OpenGL_Object#Object_zero
  return 0;
}

void destroy_shader(const uint32_t shader)
{
  glDeleteShader(shader);
}
