#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>

#include "video.h"

Video::Video() {
    window_width = 1280;
    window_height = 720;
}

bool Video::initialize() {
    if (!initWindow())
        return false;

    if (!initGraphics())
        return false;

    return true;
}

bool Video::initWindow() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init video: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Test Window", window_width, window_height, 0);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init window: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool Video::initGraphics() {
    gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

    if (!SDL_ClaimWindowForGPUDevice(gpuDevice, window)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not claim window with GPU Device: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void Video::render(Vector2 cameraPos, Vector2 cameraScale, const std::vector<RenderInfo> &objects) {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);

    SDL_GPUTexture* swapchain;

    SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain, &window_width, &window_height);

    SDL_GPUColorTargetInfo colors = {};
    colors.texture = swapchain;
    colors.load_op = SDL_GPU_LOADOP_CLEAR;
    colors.store_op = SDL_GPU_STOREOP_STORE;
    colors.clear_color = {0, 0, 0, 1};
    SDL_GPURenderPass *render = SDL_BeginGPURenderPass(cmd, &colors, 1, NULL);

    SDL_EndGPURenderPass(render);

    SDL_SubmitGPUCommandBuffer(cmd);
}
