#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} AppState;

#endif // !MAIN_H
