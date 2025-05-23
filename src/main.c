#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "helper.h"

#define NAVY 0x2A4759FF
#define ORANGE 0xF79B72FF
#define GREY 0xDDDDDDFF

int W = 16 * 75,
    H = 9 * 75;
bool clicked = false;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} AppState;

static int frameCount = 0;
static Uint64 then = 0;
int score = 0;

#define InitialRadius 64
#define InitialThickness 8

struct {
    float x, y, radius, thickness;
    bool hit;
} hitCircle = {
    .radius = InitialRadius,
    .thickness = InitialThickness,
    .hit = false,
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_Abort("Couldn't initialize SDL");
    }

    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_Abort("Failed appstate allocation");
    }
    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("osu!", W, H, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer)) {
        return SDL_Abort("Failed to create window/renderer");
    }
    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);

    hitCircle.x = W / 2.0f;
    hitCircle.y = H / 2.0f;

    return SDL_APP_CONTINUE;
}

#define MAX_CIRCLES 3
#define SEGMENTS 64
#define VERTS_PER_CIRCLE (SEGMENTS + 2)
#define INDICES_PER_CIRCLE (SEGMENTS * 3)

SDL_Vertex vertices[MAX_CIRCLES * VERTS_PER_CIRCLE];
int indices[MAX_CIRCLES * INDICES_PER_CIRCLE];
int vertCount = 0;
int indexCount = 0;

void add_circle(float cx, float cy, float radius, SDL_FColor color)
{
    int baseVert = vertCount;

    // Center point
    vertices[vertCount++] = (SDL_Vertex) {
        .position = { cx, cy },
        .color = color,
        .tex_coord = { 0, 0 }
    };

    // Ring points
    for (int i = 0; i <= SEGMENTS; ++i) {
        float angle = (float)i / SEGMENTS * 2.0f * SDL_PI_F;
        float x = cx + cosf(angle) * radius;
        float y = cy + sinf(angle) * radius;

        vertices[vertCount++] = (SDL_Vertex) {
            .position = { x, y },
            .color = color,
            .tex_coord = { 0, 0 }
        };

        if (i == 0)
            continue;
        // Indices for triangle fan
        indices[indexCount++] = baseVert; // center
        indices[indexCount++] = baseVert + i; // current edge
        indices[indexCount++] = baseVert + i + 1; // next edge
    }
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;
    SDL_SetRenderDrawColor(as->renderer, RGBA(NAVY));
    SDL_RenderClear(as->renderer);

    vertCount = 0;
    indexCount = 0;

    float mx, my;
    SDL_GetMouseState(&mx, &my);
    add_circle(mx, my, 8, (SDL_FColor)RGBA_F(0xFFFFFF55));

    if (clicked) {
        float distance = (mx - hitCircle.x) * (mx - hitCircle.x) + (my - hitCircle.y) * (my - hitCircle.y);
        if (distance <= hitCircle.radius * hitCircle.radius) {
            hitCircle.hit = true;
            score++;
        }
        clicked = false;
    }

    if (hitCircle.hit) {
        hitCircle.x = SDL_rand(W - 2 * InitialRadius) + InitialRadius, hitCircle.y = SDL_rand(H - 2 * InitialRadius) + InitialRadius;
        hitCircle.radius = InitialRadius;
        hitCircle.thickness = InitialThickness;
        hitCircle.hit = false;
    } else {
        hitCircle.radius -= 0.4f;
        hitCircle.thickness -= 0.05f;
        if (hitCircle.radius <= 0) {
            hitCircle.hit = true;
        }
    }
    add_circle(hitCircle.x, hitCircle.y, hitCircle.radius, (SDL_FColor)RGBA_F(GREY));
    add_circle(hitCircle.x, hitCircle.y, hitCircle.radius - hitCircle.thickness, (SDL_FColor)RGBA_F(ORANGE));

    SDL_RenderGeometryRaw(
        as->renderer,
        NULL,
        (float *)&vertices[0].position, sizeof(SDL_Vertex),
        (SDL_FColor *)&vertices[0].color, sizeof(SDL_Vertex),
        NULL, 0,
        vertCount,
        indices,
        indexCount,
        sizeof(int));

    SDL_SetRenderDrawColor(as->renderer, RGBA(0xFFFFFFFF));
    SDL_SetRenderScale(as->renderer, 2.0f, 2.0f);
    SDL_RenderDebugTextFormat(as->renderer, 10, 10, "Score: %d", score);
    SDL_SetRenderScale(as->renderer, 1.0f, 1.0f);
    SDL_RenderPresent(as->renderer);

    Uint64 now = SDL_GetTicks();
    frameCount++;
    if (now - then >= 1000) { // 1 second
        float fps = frameCount * 1000.0f / (now - then);
        SDL_Log("FPS: %.2f", fps);
        then = now;
        frameCount = 0;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_UP:
        clicked = true;
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSize(((AppState *)appstate)->window, &W, &H);
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
        break;
    case SDL_EVENT_KEY_UP:
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
        break;
    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
