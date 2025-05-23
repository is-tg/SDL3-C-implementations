#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define LOAD_SHADER(filename, sizeptr) SDL_LoadFile("C:\\Users\\Tharun\\Documents\\projects\\solar-sim\\shaders\\" filename, sizeptr)

typedef struct {
    SDL_Window *window;
    SDL_GPUDevice *device;
    SDL_GPUBuffer *vertexBuffer;
    SDL_GPUTransferBuffer *transferBuffer;
    SDL_GPUGraphicsPipeline *graphicsPipeline;
} AppState;

#endif // !MAIN_H
