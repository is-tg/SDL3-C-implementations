#include "render.h"

#define SEGMENTS 64
#define MAX_CIRCLES 3
#define VERTS_PER_CIRCLE (SEGMENTS + 2)
#define INDICES_PER_CIRCLE (SEGMENTS * 3)

static SDL_Vertex vertices[MAX_CIRCLES * VERTS_PER_CIRCLE];
static int indices[MAX_CIRCLES * INDICES_PER_CIRCLE];
static int vertCount = 0;
static int indexCount = 0;

void render_reset_buffers(void)
{
    vertCount = 0;
    indexCount = 0;
}

void add_circle(float cx, float cy, float radius, SDL_FColor color)
{
    if ((vertCount + VERTS_PER_CIRCLE) > MAX_CIRCLES * VERTS_PER_CIRCLE || (indexCount + INDICES_PER_CIRCLE) > MAX_CIRCLES * INDICES_PER_CIRCLE) {
        SDL_Log("No more circles!");
        return;
    }
    int baseVert = vertCount;
    vertices[vertCount++] = (SDL_Vertex) { .position = { cx, cy }, .color = color };

    for (int i = 0; i <= SEGMENTS; ++i) {
        float angle = (float)i / SEGMENTS * 2.0f * SDL_PI_F;
        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;
        vertices[vertCount++] = (SDL_Vertex) { .position = { x, y }, .color = color };

        if (i > 0) {
            indices[indexCount++] = baseVert;
            indices[indexCount++] = baseVert + i;
            indices[indexCount++] = baseVert + i + 1;
        }
    }
}

void flush_render(SDL_Renderer *renderer)
{
    SDL_RenderGeometryRaw(renderer, NULL,
        (float *)&vertices[0].position, sizeof(SDL_Vertex),
        (SDL_FColor *)&vertices[0].color, sizeof(SDL_Vertex),
        NULL, 0, vertCount,
        indices, indexCount, sizeof(int));
}
