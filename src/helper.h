#ifndef HELPER_H
#define HELPER_H

#include "SDL3/SDL.h"

#define RGBA(hex) ((hex >> 24) & 0xFF), ((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)
#define RGBA_F(hex) ((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f, ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f

SDL_AppResult SDL_Abort(char *report);

#endif
