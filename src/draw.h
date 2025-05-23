#ifndef DRAW_H
#define DRAW_H

#include <SDL3/SDL.h>

typedef struct {
    int x, y;
    int r;
    Uint32 color;
} Circle;

void drawCircle(Uint32 *pixels, int pitch, Circle *circle);

#endif // !DRAW_H
