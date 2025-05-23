#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "main.h"
#include "helper.h"
#include "game.h"

static int frameCount = 0;
static Uint64 lastTime = 0;
static Uint64 lastPrint = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_Abort("Couldn't initialize SDL");
    }

    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_Abort("Failed appstate allocation");
    }
    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("osu!", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &as->window, &as->renderer)) {
        return SDL_Abort("Failed to create window/renderer");
    }

    as->texture = SDL_CreateTexture(as->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!as->texture) {
        return SDL_Abort("Couldn't create streaming texture");
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
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

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;
    SDL_Surface *surface;

    if (!SDL_LockTextureToSurface(as->texture, NULL, &surface)) {
        SDL_Log("Failed to lock texture to surface, continuing...");
        return SDL_APP_CONTINUE;
    }

    SDL_FillSurfaceRect(surface, NULL, RGB_FMT(surface, 0x000000));

    draw(surface);

    SDL_UnlockTexture(as->texture);
    SDL_RenderTexture(as->renderer, as->texture, NULL, NULL);
    SDL_RenderPresent(as->renderer);

    frameCount++;
    Uint64 now = SDL_GetTicks();

    if (now - lastPrint >= 1000) { // 1 second
        float fps = frameCount * 1000.0f / (now - lastPrint);
        SDL_Log("FPS: %.2f", fps);
        lastPrint = now;
        frameCount = 0;
    }

    SDL_Delay(16);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyTexture(as->texture);
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
