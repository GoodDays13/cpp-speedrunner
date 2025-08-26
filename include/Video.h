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
    const SDL_DisplayMode* getDisplayMode();
private:
    struct Vertex {
        Vector2 position;
    };
    struct IndexTriangle {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };

    SDL_Window* window;
    SDL_GPUDevice* gpuDevice;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    SDL_GPUBuffer* miscBuffer;
    SDL_GPUTransferBuffer* miscTransferBuffers[3];
    SDL_GPUFence* inFlightFrames[3];
    unsigned int frameIndex = 0;

    bool initWindow();
    bool initGraphics();
    void createPipeline();
    void loadMeshes();
};
