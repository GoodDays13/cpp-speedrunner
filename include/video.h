#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <vector>

#include "math.h"

using MeshHandle = unsigned int;

class Video {
public:
    enum Model {
        SQUARE
    };
    struct RenderInfo {
        Model model;
        Vector2 position;
        Vector2 scale;
    };

    unsigned int window_width;
    unsigned int window_height;

    Video();
    bool initialize();
    void render(Vector2 cameraPos, Vector2 cameraScale, const std::vector<RenderInfo> &objects);
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

    bool initWindow();
    bool initGraphics();
};
