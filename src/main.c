#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#define ASPECT_RATIO_FACTOR 75
#define WINDOW_WIDTH 16 * ASPECT_RATIO_FACTOR
#define WINDOW_HEIGHT 9 * ASPECT_RATIO_FACTOR

#define NAVY 0x2A4759
#define ORANGE 0xF79B72
#define GREY 0xDDDDDD

typedef struct {
    SDL_Window *window;
    SDL_GPUDevice *gpu;
} AppState;

SDL_FColor color(Uint32 hex, float alpha)
{
    return (SDL_FColor) {
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f,
        alpha,
    };
}

SDL_AppResult SDL_Abort(char *report)
{
    SDL_Log("%s\n%s", report, SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_Abort("Couldn't initialize SDL");
    }

    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_Abort("Failed appstate allocation");
    }
    *appstate = as;

    as->window = SDL_CreateWindow("title", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!as->window) {
        return SDL_Abort("Couldn't create window");
    }

    as->gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (!as->gpu) {
        return SDL_Abort("Couldn't create gpu device");
    }

    if (!SDL_ClaimWindowForGPUDevice(as->gpu, as->window)) {
        SDL_Abort("Couldn't claim window for gpu device");
    }

    SDL_GPUGraphicsPipelineCreateInfo pipeline_createinfo = {
        .vertex_shader = NULL,
        .fragment_shader = NULL,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = (SDL_GPUGraphicsPipelineTargetInfo) {
            .num_color_targets = 1,
            .color_target_descriptions = &(SDL_GPUColorTargetDescription) {
                .format = SDL_GetGPUSwapchainTextureFormat(as->gpu, as->window),
            } }
    };
    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(as->gpu, &pipeline_createinfo);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
    case SDL_EVENT_KEY_UP:
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        } else {
            break;
        }
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;

    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(as->gpu);
    if (!cmd_buf) {
        return SDL_Abort("SDL_AcquireGPUCommandBuffer failed");
    }

    SDL_GPUTexture *swapchain_tex;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, as->window, &swapchain_tex, NULL, NULL)) {
        return SDL_Abort("SDL_WaitAndAcquireGPUSwapchainTexture failed");
    }

    if (swapchain_tex) {
        SDL_GPUColorTargetInfo color_target = {
            .texture = swapchain_tex,
            .clear_color = color(NAVY, 1.0f),
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
        };
        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target, 1, NULL);

        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buf);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyGPUDevice(as->gpu);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
