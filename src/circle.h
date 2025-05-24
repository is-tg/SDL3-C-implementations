#pragma once
#include <SDL3/SDL.h>

typedef enum {
    CIRCLE_HIT,
    CIRCLE_DYING
} CircleState;

typedef struct {
    float x, y, radius, thickness;
    CircleState state;
    SDL_FColor color;
    Uint64 birth;
} Circle;

void circle_reset(Circle *c);
void circle_update(Circle *c);
bool circle_is_hit(Circle *c, float mx, float my);

extern Circle hitCircle;
