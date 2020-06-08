#pragma once
#include <stdbool.h>

unsigned int create_shader_program();
void attach_shader(const unsigned int program, const unsigned int shader);
bool link_program(const unsigned int program);
void destroy_shader_program(const unsigned int program);
