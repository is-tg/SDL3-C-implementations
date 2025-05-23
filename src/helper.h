#ifndef HELPER_H
#define HELPER_H

#include "SDL3/SDL.h"

#define RGB(hex) (((hex) >> 16) & 0xFF), (((hex) >> 8) & 0xFF), ((hex) & 0xFF)
#define RGBA(hex) ((hex >> 24) & 0xFF), ((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)
#define RGBA_F(hex) ((hex >> 24) & 0xFF) / 255.0f, ((hex >> 16) & 0xFF) / 255.0f, ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f

#define RGB_FMT(surface, color) SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, RGB(color))

SDL_AppResult SDL_Abort(char *report);

#endif
