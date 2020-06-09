#pragma once

#include <SDL2/SDL.h>

SDL_Window * create_window(int height, int width);
void destroy_window(SDL_Window *window);
