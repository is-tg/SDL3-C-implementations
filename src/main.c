#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "helper.h"
#include "circle.h"
#include "render.h"
#include "constants.h"
#include "storage.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} AppState;

static int frameCount = 0;
static float fps = 0;
static Uint64 then = 0;

static bool start = false;
static bool clicked = false;
static int score = 0;
static int highscore = 0;

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

    if (!SDL_CreateWindowAndRenderer("osu!", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer)) {
        return SDL_Abort("Failed to create window/renderer");
    }
    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);

    circle_reset(&hitCircle);
    LoadHighScore(&highscore);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (!start) {
            start = true;
            hitCircle.birth = SDL_GetTicks();
        }
        clicked = true;
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSize(((AppState *)appstate)->window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
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

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;
    SDL_SetRenderDrawColor(as->renderer, RGBA(NAVY));
    SDL_RenderClear(as->renderer);

    if (!start) {
        const char *message = "CLICK TO START";
        SDL_SetRenderDrawColor(as->renderer, RGBA(0xFFFFFFFF));

        float scale = 6.0f;
        SDL_SetRenderScale(as->renderer, scale, scale);

        // Approximate text width: 8 pixels per char, adjust as needed
        float text_width = SDL_strlen(message) * 8.0f;
        float text_height = 8.0f;

        // Convert window size to scaled coordinates
        float cx = (WINDOW_WIDTH / scale - text_width) / 2.0f;
        float cy = (WINDOW_HEIGHT / scale - text_height) / 2.0f;

        SDL_RenderDebugText(as->renderer, cx, cy, message);
        SDL_SetRenderScale(as->renderer, 1.0f, 1.0f);
        SDL_RenderPresent(as->renderer);
        return SDL_APP_CONTINUE;
    }

    render_reset_buffers();

    float mx, my;
    SDL_GetMouseState(&mx, &my);
    add_circle(mx, my, 8, (SDL_FColor)RGBA_F(0xFFFFFF55));

    if (clicked) {
        if (circle_is_hit(&hitCircle, mx, my)) {
            hitCircle.state = CIRCLE_HIT;
            score++;
        }
        clicked = false;
    }

    if (hitCircle.state == CIRCLE_DYING && hitCircle.radius <= 0) {
        if (score > highscore) {
            highscore = score;
            SaveHighScore(highscore);
        }
        score = 0;
        circle_reset(&hitCircle);
    }

    circle_update(&hitCircle);
    add_circle(hitCircle.x, hitCircle.y, hitCircle.radius, (SDL_FColor) { RGB(GREY), hitCircle.color.a });
    add_circle(hitCircle.x, hitCircle.y, hitCircle.radius - hitCircle.thickness, hitCircle.color);

    flush_render(as->renderer);

    SDL_SetRenderDrawColor(as->renderer, RGBA(0xFFFFFFFF));
    SDL_SetRenderScale(as->renderer, 2.0f, 2.0f);
    SDL_RenderDebugTextFormat(as->renderer, 10, 10, "HighScore: %d", highscore);
    SDL_RenderDebugTextFormat(as->renderer, 10, 25, "Score: %d", score);
    SDL_SetRenderScale(as->renderer, 1.0f, 1.0f);
    SDL_RenderDebugTextFormat(as->renderer, 10, WINDOW_HEIGHT - 10, "FPS: %.2f", fps);

    SDL_RenderPresent(as->renderer);

    Uint64 now = SDL_GetTicks();
    frameCount++;
    if (now - then >= 1000) { // 1 second
        fps = frameCount * 1000.0f / (now - then);
        then = now;
        frameCount = 0;
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
