#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "main.h"
#include "helper.h"

typedef struct {
    float x, y, z;
    float r, g, b, a;
} GPUVertex;

static GPUVertex vertices[] = {
    { 0.0f, 0.5f, 0.0f, RGBA_F(0xFF0000FF) },
    { -0.5f, -0.5f, 0.0f, RGBA_F(0x00FF00FF) },
    { 0.5f, -0.5f, 0.0f, RGBA_F(0x0000FFFF) }
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (as == NULL) {
        return SDL_Abort("Failed appstate allocation");
    }
    *appstate = as;

    as->window = SDL_CreateWindow("gpu time", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (as->window == NULL) {
        return SDL_Abort("Failed to create window");
    }

    as->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (as->device == NULL) {
        return SDL_Abort("Failed to create GPU device");
    }

    if (!SDL_ClaimWindowForGPUDevice(as->device, as->window)) {
        return SDL_Abort("GPUClaimWindow failed");
    }

    SDL_GPUBufferCreateInfo bufferInfo = {
        .size = sizeof(vertices),
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX
    };
    as->vertexBuffer = SDL_CreateGPUBuffer(as->device, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo = {
        .size = sizeof(vertices),
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD
    };
    as->transferBuffer = SDL_CreateGPUTransferBuffer(as->device, &transferInfo);

    GPUVertex *data = (GPUVertex *)SDL_MapGPUTransferBuffer(as->device, as->transferBuffer, false);
    SDL_memcpy(data, vertices, sizeof(vertices));

    SDL_UnmapGPUTransferBuffer(as->device, as->transferBuffer);

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(as->device);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTransferBufferLocation location = {
        .transfer_buffer = as->transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion region = {
        .buffer = as->vertexBuffer,
        .size = sizeof(vertices),
        .offset = 0
    };

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    size_t vertexCodeSize;
    void *vertexCode = LOAD_SHADER("vertex.spv", &vertexCodeSize);

    SDL_GPUShaderCreateInfo vertexInfo = {
        .code = (Uint8 *)vertexCode, // array of bytes
        .code_size = vertexCodeSize,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX
    };
    SDL_GPUShader *vertexShader = SDL_CreateGPUShader(as->device, &vertexInfo);
    SDL_free(vertexCode);

    size_t fragmentCodeSize;
    void *fragmentCode = LOAD_SHADER("fragment.spv", &fragmentCodeSize);

    SDL_GPUShaderCreateInfo fragmentInfo = {
        .code = (Uint8 *)fragmentCode, // array of bytes
        .code_size = fragmentCodeSize,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT
    };
    SDL_GPUShader *fragmentShader = SDL_CreateGPUShader(as->device, &fragmentInfo);
    SDL_free(fragmentCode);

    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1] = {
        { .slot = 0,
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
            .pitch = sizeof(GPUVertex) }
    };

    SDL_GPUVertexAttribute vertexAttributes[2] = {
        { .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0 },
        { .buffer_slot = 0,
            .location = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
            .offset = sizeof(float) * 3 }
    };

    SDL_GPUColorTargetDescription colorTargetDescriptions[1] = {
        { .format = SDL_GetGPUSwapchainTextureFormat(as->device, as->window) }
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_input_state.num_vertex_buffers = 1,
        .vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions,
        .vertex_input_state.num_vertex_attributes = 2,
        .vertex_input_state.vertex_attributes = vertexAttributes,
        .target_info.num_color_targets = 1,
        .target_info.color_target_descriptions = colorTargetDescriptions
    };
    as->graphicsPipeline = SDL_CreateGPUGraphicsPipeline(as->device, &pipelineInfo);

    SDL_ReleaseGPUShader(as->device, vertexShader);
    SDL_ReleaseGPUShader(as->device, fragmentShader);

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

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(as->device);
    if (commandBuffer == NULL) {
        return SDL_Abort("Acquire GPUCommandBuffer failed");
    }

    SDL_GPUTexture *swapchainTexture;
    Uint32 width, height;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, as->window, &swapchainTexture, &width, &height)) {
        return SDL_Abort("WaitAndAcquireGPUSwapchainTexture failed");
    }

    if (swapchainTexture != NULL) {
        SDL_GPUColorTargetInfo colorTargetInfo = {
            .clear_color = { RGBA_F(0x0C0C0CFF) }, // blacky
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .texture = swapchainTexture
        };

        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

        SDL_BindGPUGraphicsPipeline(renderPass, as->graphicsPipeline);
        SDL_GPUBufferBinding bufferBindings[1] = {
            { .buffer = as->vertexBuffer,
                .offset = 0 }
        };
        SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(commandBuffer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL) {
        AppState *as = (AppState *)appstate;

        SDL_ReleaseGPUBuffer(as->device, as->vertexBuffer);
        SDL_ReleaseGPUTransferBuffer(as->device, as->transferBuffer);
        SDL_ReleaseGPUGraphicsPipeline(as->device, as->graphicsPipeline);

        SDL_DestroyWindow(as->window);
        SDL_DestroyGPUDevice(as->device);
        SDL_free(as);
    }
}
