#include "circle.h"
#include "helper.h"
#include "constants.h"

#define MAX_RADIUS 64
#define MAX_THICKNESS 8
#define COLOR (SDL_FColor) RGBA_F(ORANGE)

Circle hitCircle;

void circle_reset(Circle *c)
{
    c->x = SDL_rand(WINDOW_WIDTH - 2 * MAX_RADIUS) + MAX_RADIUS;
    c->y = SDL_rand(WINDOW_HEIGHT - 2 * MAX_RADIUS) + MAX_RADIUS;
    c->radius = MAX_RADIUS;
    c->thickness = MAX_THICKNESS;
    c->state = CIRCLE_DYING;
    c->color = COLOR;
    c->birth = SDL_GetTicks();
}

void circle_update(Circle *c)
{
    if (c->state == CIRCLE_DYING) {
        Uint64 t = SDL_GetTicks() - c->birth;
        c->radius = MAX_RADIUS * SDL_cosf(0.001 * t);
        c->thickness *= 0.99f;
    } else {
        c->radius *= 1.08f;
        c->color.a -= 0.2f;
        if (c->color.a <= 0) {
            circle_reset(c);
        }
    }
}

bool circle_is_hit(Circle *c, float mx, float my)
{
    float dx = mx - c->x;
    float dy = my - c->y;
    return (dx * dx + dy * dy) <= c->radius * c->radius;
}
