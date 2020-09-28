#pragma once
#include <stdbool.h>

uint32_t create_shader_program();
void attach_shader(const uint32_t program, const uint32_t shader);
bool link_program(const uint32_t program);
void destroy_shader_program(const uint32_t program);
