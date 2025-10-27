#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <map>
#include <string>
#include <unordered_map>
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
    class Spritesheet {
    private:
        SDL_GPUTextureSamplerBinding *textureSampler;
    public:
        // Amount of sprites wide
        Uint8 width;
        // Amount of sprites tall
        Uint8 height;
    };
    enum Model {
        QUAD,
        SPRITE,
    };
    struct MiscData {
        Transform transform;
        Vector4 color;
        unsigned int id;
    };
    struct RenderKey {
        Model model;
        std::string spritesheet;

        bool operator<(const RenderKey& other) const {
            if (model != other.model) return model < other.model;
            return spritesheet < other.spritesheet;
        }
    };
    struct RenderInfo {
        std::map<RenderKey, std::vector<MiscData>> renderBatches;
        Transform camera;
    };

    Video();
    ~Video();
    bool initialize();
    void render(RenderInfo info);
    const SDL_DisplayMode* getDisplayMode() const;
    Vector2 convertPixelToGame(Vector2 pixel, Transform camera) const;
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

    bool conwayActive = false;

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
    std::unordered_map<std::string, Spritesheet> spritesheets;

    bool initWindow();
    bool initGraphics();
    void loadShaders();
    void createPipeline();
    void createBuffersAndGeometry();
    void initIntermediateTextures();
    void cleanup();
};
