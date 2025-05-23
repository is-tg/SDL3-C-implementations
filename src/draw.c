#include "draw.h"
#include "main.h"

void drawCircle(Uint32 *pixels, int pitch, Circle *circle)
{
    int cx = circle->x, cy = circle->y, r = circle->r;
    Uint32 color = circle->color;
    int r2 = r * r, area = r2 << 2, rr = r << 1;

    for (int i = 0; i < area; i++) {
        int tx = (i % rr) - r;
        int ty = (i / rr) - r;
        int x = cx + tx;
        int y = cy + ty;

        if ((x >= 0) && (x < WINDOW_WIDTH) && (y >= 0) && (y < WINDOW_HEIGHT)) {
            if (tx * tx + ty * ty <= r2) {
                pixels[y * pitch + x] = color;
            }
        }
    }
}
