#define SDL_MAIN_USE_CALLBACKS
#include "main.h"
#include "helper.h"
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_AppFail("Failed appstate allocation");
    }
    *appstate = as;

    as->window = SDL_CreateWindow("gpu time", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!as->window) {
        return SDL_AppFail("Failed to create window");
    }

    as->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
    if (!as->device) {
        return SDL_AppFail("Failed to create GPU context");
    }
    SDL_ClaimWindowForGPUDevice(as->device, as->window);

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
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyWindow(as->window);
        SDL_DestroyGPUDevice(as->device);
        SDL_free(as);
    }
}
