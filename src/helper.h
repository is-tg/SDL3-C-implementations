#ifndef HELPER_H
#define HELPER_H

#include "SDL3/SDL.h"

#define RGBA(hex) ((hex >> 24) & 0xFF), ((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)

SDL_AppResult SDL_AppFail(char *report);

#endif
