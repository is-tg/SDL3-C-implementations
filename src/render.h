#pragma once
#include <SDL3/SDL.h>

void add_circle(float cx, float cy, float radius, SDL_FColor color);
void flush_render(SDL_Renderer *renderer);

void render_reset_buffers(void);
