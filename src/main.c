#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define ASPECT_RATIO_FACTOR 75
#define WINDOW_WIDTH 16 * ASPECT_RATIO_FACTOR
#define WINDOW_HEIGHT 9 * ASPECT_RATIO_FACTOR

#define NAVY 0x2A4759
#define ORANGE 0xF79B72
#define GREY 0xDDDDDD

typedef struct {
    SDL_Window *window;
    SDL_GPUDevice *gpu;
    SDL_GPUGraphicsPipeline *pipeline;
} AppState;

SDL_FColor color(Uint32 hex, float alpha);
SDL_AppResult SDL_Abort(const char *report);
SDL_GPUShader *LoadShader(SDL_GPUDevice *device, const char *file, SDL_GPUShaderStage stage);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_Abort("Couldn't initialize SDL");
    }

    AppState *as = (AppState *) SDL_calloc(1, sizeof(AppState));
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
        return SDL_Abort("Couldn't claim window for gpu device");
    }

    SDL_GPUShader *vertex_shader = LoadShader(as->gpu, "shader.spv.vert", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader *fragment_shader = LoadShader(as->gpu, "shader.spv.frag", SDL_GPU_SHADERSTAGE_FRAGMENT);

    SDL_GPUGraphicsPipelineCreateInfo pipeline_createinfo = {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = (SDL_GPUGraphicsPipelineTargetInfo) {
            .num_color_targets = 1,
            .color_target_descriptions = &(SDL_GPUColorTargetDescription) {
                .format = SDL_GetGPUSwapchainTextureFormat(as->gpu, as->window),
            }
        }
    };
    as->pipeline = SDL_CreateGPUGraphicsPipeline(as->gpu, &pipeline_createinfo);

    SDL_ReleaseGPUShader(as->gpu, vertex_shader);
    SDL_ReleaseGPUShader(as->gpu, fragment_shader);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void) appstate;

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
    AppState *as = (AppState *) appstate;

    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(as->gpu);
    if (!cmd_buf) {
        return SDL_Abort("SDL_AcquireGPUCommandBuffer failed");
    }

    SDL_GPUTexture *swapchain_tex;
    SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, as->window, &swapchain_tex, NULL, NULL);

    if (swapchain_tex) {
        SDL_GPUColorTargetInfo color_target = {
            .texture = swapchain_tex,
            .clear_color = color(NAVY, 1.0f),
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
        };
        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target, 1, NULL);
        SDL_BindGPUGraphicsPipeline(render_pass, as->pipeline);

        SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buf);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void) result;

    if (appstate) {
        AppState *as = (AppState *) appstate;
        SDL_DestroyGPUDevice(as->gpu);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}

SDL_FColor color(Uint32 hex, float alpha)
{
    return (SDL_FColor) {
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f,
        alpha,
    };
}

SDL_GPUShader *LoadShader(SDL_GPUDevice *device, const char *file, SDL_GPUShaderStage stage)
{
    size_t codesize;
    void *code = SDL_LoadFile(file, &codesize);
    if (!code) {
        SDL_Log("Failed to load shader file: %s", file);
    }
    SDL_GPUShaderCreateInfo shader_createinfo = {
        .code_size = codesize,
        .code = (Uint8 *) code,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,

    };
    SDL_GPUShader *shader = SDL_CreateGPUShader(device, &shader_createinfo);
    SDL_free(code);

    return shader;
}

SDL_AppResult SDL_Abort(const char *report)
{
    SDL_Log("%s\n%s", report, SDL_GetError());
    return SDL_APP_FAILURE;
}
