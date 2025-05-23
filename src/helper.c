#include "helper.h"

SDL_AppResult SDL_Abort(char *report)
{
    SDL_Log("%s: %s", report, SDL_GetError());
    return SDL_APP_FAILURE;
}
