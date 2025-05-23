#include "game.h"
#include "draw.h"
#include "helper.h"

Circle cursor = {
    .r = 50
};

void draw(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;
    int pitch = surface->pitch / sizeof(Uint32);

    float mx, my;
    SDL_GetMouseState(&mx, &my);
    cursor.x = (int)mx;
    cursor.y = (int)my;
    cursor.color = RGB_FMT(surface, 0xFFFFFF);

    drawCircle(pixels, pitch, &cursor);
}
