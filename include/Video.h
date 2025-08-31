#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <vector>

#include "math.h"

#define SHADER_VERTEX shaders[0]
#define SHADER_FRAGMENT shaders[1]
#define SHADER_FULLSCREEN shaders[2]
#define SHADER_BOXBLUR shaders[3]
#define SHADER_CONWAY shaders[4]

using MeshHandle = unsigned int;

class Video {
public:
    enum Model {
        SQUARE
    };
    struct MiscData {
        Vector2 position;
        Vector2 scale;
        Vector4 color;
    };
    struct InstanceInfo {
        Model model;
        MiscData data;
    };
    struct RenderInfo {
        std::vector<InstanceInfo> instances;
        Vector2 cameraPosition;
        Vector2 cameraScale;
    };

    Video();
    ~Video();
    bool initialize();
    void render(RenderInfo info);
    const SDL_DisplayMode* getDisplayMode() const;
    Vector2 convertPixelToGame(Vector2 pixel, Vector2 cameraPos, Vector2 cameraScale) const;
private:
    struct Vertex {
        Vector2 position;
        Vector2 uv;
    };
    struct IndexTriangle {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };

    unsigned int window_width;
    unsigned int window_height;

    SDL_Window* window;
    SDL_GPUDevice* gpuDevice;
    SDL_GPUShader* shaders[5];
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    SDL_GPUGraphicsPipeline* boxBlurPipeline;
    SDL_GPUGraphicsPipeline* conwayPipeline;
    SDL_GPUTexture* intermediates[2];
    unsigned int currentIntermediate = 0;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    SDL_GPUBuffer* miscBuffer;
    SDL_GPUTransferBuffer* miscTransferBuffers[3];
    SDL_GPUFence* inFlightFrames[3] = {nullptr, nullptr, nullptr};
    unsigned int frameIndex = 0;

    bool initWindow();
    bool initGraphics();
    void createPipeline();
    void loadMeshes();
    void initIntermediateTextures();
    void cleanup();
};
