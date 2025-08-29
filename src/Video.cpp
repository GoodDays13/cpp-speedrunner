#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>
#include <cmath>

#include "Video.h"
#include "shaders.h"

Video::Video() : window_width(1280), window_height(720) {}

Video::~Video() {
    cleanup();
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

    window = SDL_CreateWindow("Speedrunner", window_width, window_height, 0);
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

    if (SDL_WindowSupportsGPUPresentMode(gpuDevice, window, SDL_GPU_PRESENTMODE_MAILBOX))
        SDL_SetGPUSwapchainParameters(gpuDevice, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

    // Load shaders
    SDL_GPUShaderCreateInfo shader_info = {};
    shader_info.code_size = vertex_vert_spv_len;
    shader_info.code = vertex_vert_spv;
    shader_info.entrypoint = "main";
    shader_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shader_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    shader_info.num_uniform_buffers = 1;

    SHADER_VERTEX = SDL_CreateGPUShader(gpuDevice, &shader_info);

    shader_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    shader_info.code_size = fragment_frag_spv_len;
    shader_info.code = fragment_frag_spv;

    SHADER_FRAGMENT = SDL_CreateGPUShader(gpuDevice, &shader_info);

    shader_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    shader_info.code_size = fullscreen_vert_spv_len;
    shader_info.code = fullscreen_vert_spv;

    SHADER_FULLSCREEN = SDL_CreateGPUShader(gpuDevice, &shader_info);

    shader_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    shader_info.code_size = boxblur_frag_spv_len;
    shader_info.code = boxblur_frag_spv;
    shader_info.num_samplers = 1;

    SHADER_BOXBLUR = SDL_CreateGPUShader(gpuDevice, &shader_info);

    shader_info.code_size = conway_frag_spv_len;
    shader_info.code = conway_frag_spv;

    SHADER_CONWAY = SDL_CreateGPUShader(gpuDevice, &shader_info);

    // Create graphics pipeline
    createPipeline();

    // Load static info into buffers
    loadMeshes();

    initIntermediateTextures();

    return true;
}

void Video::createPipeline() {
    SDL_GPUGraphicsPipelineCreateInfo pipelineinfo = {};
    pipelineinfo.vertex_shader = SHADER_VERTEX;
    pipelineinfo.fragment_shader = SHADER_FRAGMENT;

    SDL_GPUVertexBufferDescription vertex_buffer_descriptions[2] = {};
    pipelineinfo.vertex_input_state.num_vertex_buffers = 2;
    SDL_GPUVertexAttribute vertex_attributes[4] = {};
    pipelineinfo.vertex_input_state.num_vertex_attributes = 4;
    pipelineinfo.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions;
    pipelineinfo.vertex_input_state.vertex_attributes = vertex_attributes;

    // static vertex buffer
    vertex_buffer_descriptions[0].slot = 0;
    vertex_buffer_descriptions[0].pitch = sizeof(Vertex);
    vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].buffer_slot = 0;
    vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].buffer_slot = 0;
    vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertex_attributes[1].offset = sizeof(Vector2);

    // misc instance buffer
    vertex_buffer_descriptions[1].slot = 1;
    vertex_buffer_descriptions[1].pitch = sizeof(Vector2) * 2;
    vertex_buffer_descriptions[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
    vertex_attributes[2].location = 2;
    vertex_attributes[2].buffer_slot = 1;
    vertex_attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertex_attributes[3].location = 3;
    vertex_attributes[3].buffer_slot = 1;
    vertex_attributes[3].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertex_attributes[3].offset = sizeof(Vector2);

    pipelineinfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUColorTargetDescription color_target = {};
    pipelineinfo.target_info.num_color_targets = 1;
    pipelineinfo.target_info.color_target_descriptions = &color_target;
    color_target.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;

    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineinfo);

    pipelineinfo.vertex_shader = SHADER_FULLSCREEN;
    pipelineinfo.fragment_shader = SHADER_BOXBLUR;
    pipelineinfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineinfo.vertex_input_state.num_vertex_attributes = 2;

    boxBlurPipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineinfo);

    pipelineinfo.fragment_shader = SHADER_CONWAY;

    conwayPipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineinfo);
}

void Video::loadMeshes() {
    SDL_GPUBufferCreateInfo vertexInfo = {};
    SDL_GPUBufferCreateInfo indexInfo = {};
    SDL_GPUBufferCreateInfo miscInfo = {};
    SDL_GPUTransferBufferCreateInfo miscTransferInfo = {};

    vertexInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexInfo.size = sizeof(Vertex) * 4;

    indexInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indexInfo.size = sizeof(IndexTriangle) * 2;

    miscInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    miscInfo.size = std::pow(2, 20);

    miscTransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    miscTransferInfo.size = std::pow(2, 20);

    vertexBuffer = SDL_CreateGPUBuffer(gpuDevice, &vertexInfo);
    indexBuffer = SDL_CreateGPUBuffer(gpuDevice, &indexInfo);
    miscBuffer = SDL_CreateGPUBuffer(gpuDevice, &miscInfo);
    miscTransferBuffers[0] = SDL_CreateGPUTransferBuffer(gpuDevice, &miscTransferInfo);
    miscTransferBuffers[1] = SDL_CreateGPUTransferBuffer(gpuDevice, &miscTransferInfo);
    miscTransferBuffers[2] = SDL_CreateGPUTransferBuffer(gpuDevice, &miscTransferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = vertexInfo.size + indexInfo.size;
    SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(gpuDevice, &transferInfo);

    SDL_GPUTransferBufferLocation vertexSrc = {};
    SDL_GPUTransferBufferLocation indexSrc = {};
    SDL_GPUBufferRegion vertexDst = {};
    SDL_GPUBufferRegion indexDst = {};
    vertexSrc.transfer_buffer = transfer;
    indexSrc.transfer_buffer = transfer;
    indexSrc.offset = vertexInfo.size;
    vertexDst.buffer = vertexBuffer;
    vertexDst.size = vertexInfo.size;
    indexDst.buffer = indexBuffer;
    indexDst.size = indexInfo.size;

    void* map = SDL_MapGPUTransferBuffer(gpuDevice, transfer, false);
    Vertex* vertexMap = (Vertex*)map;
    IndexTriangle* indexMap = (IndexTriangle*)((char*)map + indexSrc.offset);

    vertexMap[0] = {{-.5, -.5}, {0, 1}};
    vertexMap[1] = {{.5, -.5}, {1, 1}};
    vertexMap[2] = {{-.5, .5}, {0, 0}};
    vertexMap[3] = {{.5, .5}, {1, 0}};

    indexMap[0] = {0, 1, 2};
    indexMap[1] = {1, 2, 3};

    SDL_UnmapGPUTransferBuffer(gpuDevice, transfer);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);
    SDL_GPUCopyPass* copypass = SDL_BeginGPUCopyPass(cmd);
    SDL_UploadToGPUBuffer(copypass, &vertexSrc, &vertexDst, false);
    SDL_UploadToGPUBuffer(copypass, &indexSrc, &indexDst, false);
    SDL_EndGPUCopyPass(copypass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(gpuDevice, transfer);
}

void Video::initIntermediateTextures() {
    SDL_GPUTextureCreateInfo intermediateInfo;
    intermediateInfo.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    intermediateInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    intermediateInfo.width = window_width;
    intermediateInfo.height = window_height;
    intermediateInfo.layer_count_or_depth = 1;
    intermediateInfo.num_levels = 1;
    intermediateInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    intermediates[0] = SDL_CreateGPUTexture(gpuDevice, &intermediateInfo);
    intermediates[1] = SDL_CreateGPUTexture(gpuDevice, &intermediateInfo);
}

void Video::render(Vector2 cameraPos, Vector2 cameraScale, const std::vector<RenderInfo> &objects) {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);

    int i = frameIndex % 3;
    if (inFlightFrames[i]) {
        SDL_WaitForGPUFences(gpuDevice, true, &inFlightFrames[i], 1);
        SDL_ReleaseGPUFence(gpuDevice, inFlightFrames[i]);
        inFlightFrames[i] = nullptr;
    }

    SDL_GPUTransferBuffer* miscTransferBuffer = miscTransferBuffers[i];

    void* miscMap = SDL_MapGPUTransferBuffer(gpuDevice, miscTransferBuffer, false);
    Vector2* vectorData = (Vector2*)miscMap;

    // TODO: Add detection for overflow of buffer
    for (int i = 0; i < objects.size(); i++) {
        vectorData[2 * i] = objects[i].position;
        vectorData[2 * i + 1] = objects[i].scale;
    }

    SDL_UnmapGPUTransferBuffer(gpuDevice, miscTransferBuffer);

    SDL_GPUCopyPass* copypass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation miscSrc = {};
    miscSrc.transfer_buffer = miscTransferBuffer;
    SDL_GPUBufferRegion miscDst = {};
    miscDst.buffer = miscBuffer;
    miscDst.size = sizeof(Vector2) * 2 * objects.size();

    SDL_UploadToGPUBuffer(copypass, &miscSrc, &miscDst, false);

    SDL_EndGPUCopyPass(copypass);

    float mapped[16];

    // ---- Camera/view matrix ----
    // 20x10 viewport centered at 100,100
    // We'll use orthographic projection: left/right/top/bottom = 90/110/-5/5
    // Scale = 2/(right-left), etc.
    float scaleX = 2.0f / cameraScale.x;
    float scaleY = 2.0f / cameraScale.y;
    float transX = - (cameraPos.x) * scaleX;
    float transY = - (cameraPos.y) * scaleY;

    // Column-major mat4
    mapped[0]  = scaleX; mapped[4]  = 0.0f;   mapped[8]  = 0.0f; mapped[12] = transX;
    mapped[1]  = 0.0f;   mapped[5]  = scaleY; mapped[9]  = 0.0f; mapped[13] = transY;
    mapped[2]  = 0.0f;   mapped[6]  = 0.0f;   mapped[10] = 1.0f; mapped[14] = 0.0f;
    mapped[3]  = 0.0f;   mapped[7]  = 0.0f;   mapped[11] = 0.0f; mapped[15] = 1.0f;

    SDL_PushGPUVertexUniformData(cmd, 0, mapped, 16 * sizeof(float));


    // BEGIN RENDER PASSES
    SDL_GPUColorTargetInfo colors = {};
    SDL_GPUBufferBinding bindings[3] = {};
    bindings[0].buffer = vertexBuffer;
    bindings[1].buffer = miscBuffer;
    bindings[2].buffer = indexBuffer;
    colors.load_op = SDL_GPU_LOADOP_CLEAR;
    colors.store_op = SDL_GPU_STOREOP_STORE;
    colors.clear_color = {0, 0, 0, 1};

    SDL_GPUViewport viewport = {};
    viewport.w = window_width;
    viewport.h = window_height;

    SDL_GPUTexture* swapchain;
    SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain, &window_width, &window_height);

    if (window_width != viewport.w || window_height != viewport.h) {
        SDL_ReleaseGPUTexture(gpuDevice, intermediates[0]);
        SDL_ReleaseGPUTexture(gpuDevice, intermediates[1]);
        initIntermediateTextures();
        viewport.w = window_width;
        viewport.h = window_height;
    };


    // Objects Pass
    colors.texture = intermediates[currentIntermediate];
    SDL_GPURenderPass *render = SDL_BeginGPURenderPass(cmd, &colors, 1, NULL);

    SDL_BindGPUGraphicsPipeline(render, graphicsPipeline);
    SDL_SetGPUViewport(render, &viewport);

    SDL_BindGPUVertexBuffers(render, 0, bindings, 2);
    SDL_BindGPUIndexBuffer(render, bindings + 2, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    SDL_DrawGPUIndexedPrimitives(render, 6, objects.size(), 0, 0, 0);

    SDL_EndGPURenderPass(render);


    // Post-Processing Passes
    // SDL_GPUSamplerCreateInfo samplerInfo = {};
    // SDL_GPUTextureSamplerBinding samplerBinding = {};
    // samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    // samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    //
    // SDL_GPUSampler* sampler = SDL_CreateGPUSampler(gpuDevice, &samplerInfo);
    // samplerBinding.sampler = sampler;
    //
    // samplerBinding.texture = intermediates[currentIntermediate];
    // currentIntermediate = (currentIntermediate + 1) % 2;
    // colors.texture = intermediates[currentIntermediate];
    //
    // render = SDL_BeginGPURenderPass(cmd, &colors, 1, NULL);
    // SDL_BindGPUGraphicsPipeline(render, conwayPipeline);
    // SDL_SetGPUViewport(render, &viewport);
    //
    // SDL_BindGPUVertexBuffers(render, 0, bindings, 1);
    // SDL_BindGPUIndexBuffer(render, bindings + 2, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    // SDL_BindGPUFragmentSamplers(render, 0, &samplerBinding, 1);
    // SDL_DrawGPUIndexedPrimitives(render, 6, 1, 0, 0, 0);
    // SDL_EndGPURenderPass(render);


    SDL_GPUTextureLocation src = {};
    SDL_GPUTextureLocation dst = {};
    src.texture = intermediates[currentIntermediate];
    dst.texture = swapchain;
    SDL_GPUCopyPass* finalCopy = SDL_BeginGPUCopyPass(cmd);
    SDL_CopyGPUTextureToTexture(finalCopy, &src, &dst, window_width, window_height, 1, false);
    SDL_EndGPUCopyPass(finalCopy);

    inFlightFrames[i] = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);

    frameIndex++;
}

const SDL_DisplayMode* Video::getDisplayMode() {
    return SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
}

void Video::cleanup() {
    SDL_ReleaseGPUFence(gpuDevice, inFlightFrames[0]);
    SDL_ReleaseGPUFence(gpuDevice, inFlightFrames[1]);
    SDL_ReleaseGPUFence(gpuDevice, inFlightFrames[2]);
    SDL_ReleaseGPUTransferBuffer(gpuDevice, miscTransferBuffers[0]);
    SDL_ReleaseGPUTransferBuffer(gpuDevice, miscTransferBuffers[1]);
    SDL_ReleaseGPUTransferBuffer(gpuDevice, miscTransferBuffers[2]);
    SDL_ReleaseGPUBuffer(gpuDevice, miscBuffer);
    SDL_ReleaseGPUBuffer(gpuDevice, indexBuffer);
    SDL_ReleaseGPUBuffer(gpuDevice, vertexBuffer);
    SDL_ReleaseGPUGraphicsPipeline(gpuDevice, graphicsPipeline);
    SDL_ReleaseGPUGraphicsPipeline(gpuDevice, boxBlurPipeline);
    SDL_ReleaseGPUGraphicsPipeline(gpuDevice, conwayPipeline);
    for (int i = 0; i < sizeof(shaders) / sizeof(shaders[0]); i++)
        SDL_ReleaseGPUShader(gpuDevice, shaders[i]);
    for (int i = 0; i < 2; i++)
        SDL_ReleaseGPUTexture(gpuDevice, intermediates[i]);
    SDL_ReleaseWindowFromGPUDevice(gpuDevice, window);
    SDL_DestroyGPUDevice(gpuDevice);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
