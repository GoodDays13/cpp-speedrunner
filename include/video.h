#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "math.h"

using MeshHandle = unsigned int;

class Video {
public:
    enum Model {
        SQUARE
    };

    Video();
    bool initialize();
private:
    struct Vertex {
        Vector2 position;
    };

    SDL_Window* window;
    SDL_GPUDevice* gpuDevice;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    SDL_GPUBuffer* miscBuffer;
};
