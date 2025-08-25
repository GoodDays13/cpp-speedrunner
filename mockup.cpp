#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>

#include "shader_vertex.h"
#include "shader_fragment_white.h"

int main(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    unsigned int window_width = 800;
    unsigned int window_height = 400;

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL3 window",                  // window title
        window_width,                               // width, in pixels
        window_height,                               // height, in pixels
        0                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GPUDevice *gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

    if (!SDL_ClaimWindowForGPUDevice(gpu, window)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not claim window with GPU Device: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GPUBufferCreateInfo bufferinfo = {};
    SDL_GPUTransferBufferCreateInfo transferinfo = {};
    bufferinfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferinfo.size = 1024;
    transferinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferinfo.size = 1024;

    SDL_GPUBuffer *gpu_buffer = SDL_CreateGPUBuffer(gpu, &bufferinfo);
    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(gpu, &transferinfo);

    void* mapped_data = SDL_MapGPUTransferBuffer(gpu, transfer, true);

    float* vertex_data = (float*)mapped_data;

    // Simple 2D quad via triangle strip
    vertex_data[0] = -0.5f; vertex_data[1] = -0.5f;
    vertex_data[2] =  0.5f; vertex_data[3] = -0.5f;
    vertex_data[4] = -0.5f; vertex_data[5] =  0.5f;
    vertex_data[6] =  0.5f; vertex_data[7] =  0.5f;

    SDL_UnmapGPUTransferBuffer(gpu, transfer);

    SDL_GPUShaderCreateInfo shader_info = {};
    shader_info.code_size = vertex_spv_len;
    shader_info.code = vertex_spv;
    shader_info.entrypoint = "main";
    shader_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shader_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    shader_info.num_uniform_buffers = 2;

    SDL_GPUShader *vertex_shader_2D = SDL_CreateGPUShader(gpu, &shader_info);

    shader_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    shader_info.code_size = fragment_white_spv_len;
    shader_info.code = fragment_white_spv;

    SDL_GPUShader *fragment_shader_white = SDL_CreateGPUShader(gpu, &shader_info);

    SDL_GPUGraphicsPipelineCreateInfo pipelineinfo = {};
    SDL_GPUVertexBufferDescription vertex_buffer_description = {};
    SDL_GPUVertexAttribute vertex_attribute = {};
    pipelineinfo.vertex_shader = vertex_shader_2D;
    pipelineinfo.fragment_shader = fragment_shader_white;
    pipelineinfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineinfo.vertex_input_state.num_vertex_attributes = 1;
    pipelineinfo.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_description;
    vertex_buffer_description.slot = 0;
    vertex_buffer_description.pitch = sizeof(float) * 2;
    vertex_buffer_description.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    pipelineinfo.vertex_input_state.vertex_attributes = &vertex_attribute;
    vertex_attribute.location = 0;
    vertex_attribute.buffer_slot = 0;
    vertex_attribute.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertex_attribute.offset = 0;
    pipelineinfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;

    SDL_GPUColorTargetDescription color_target = {};
    pipelineinfo.target_info.num_color_targets = 1;
    pipelineinfo.target_info.color_target_descriptions = &color_target;
    color_target.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; // or whatever your swapchain uses

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &pipelineinfo);

    SDL_GPUBufferCreateInfo ubo_info = {};
    ubo_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    ubo_info.size = sizeof(float) * 32; // 2 mat4 = 16 floats each
    SDL_GPUBuffer *model_buffer = SDL_CreateGPUBuffer(gpu, &ubo_info);
    SDL_GPUBuffer *view_buffer = SDL_CreateGPUBuffer(gpu, &ubo_info);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);

    SDL_GPUCopyPass *copypass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation src = {};
    src.transfer_buffer = transfer;
    SDL_GPUBufferRegion dst = {};
    dst.buffer = gpu_buffer;
    dst.size = 1024;
    SDL_UploadToGPUBuffer(copypass, &src, &dst, false);

    SDL_EndGPUCopyPass(copypass);
    SDL_SubmitGPUCommandBuffer(cmd);

    float x = 0.0f;
    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        float mapped[48];

        // ---- Camera/view matrix ----
        // 20x10 viewport centered at 100,100
        // We'll use orthographic projection: left/right/top/bottom = 90/110/-5/5
        // Scale = 2/(right-left), etc.
        float scaleX = 2.0f / 20.0f;
        float scaleY = 2.0f / 10.0f;
        float transX = - (100.0f) * scaleX;
        float transY = - (100.0f) * scaleY;

        // Column-major mat4
        mapped[0]  = scaleX; mapped[4]  = 0.0f;   mapped[8]  = 0.0f; mapped[12] = transX;
        mapped[1]  = 0.0f;   mapped[5]  = scaleY; mapped[9]  = 0.0f; mapped[13] = transY;
        mapped[2]  = 0.0f;   mapped[6]  = 0.0f;   mapped[10] = 1.0f; mapped[14] = 0.0f;
        mapped[3]  = 0.0f;   mapped[7]  = 0.0f;   mapped[11] = 0.0f; mapped[15] = 1.0f;

        // ---- Model matrix for box at 95,95 ----
        float modelX = 90.0f + x;
        float modelY = 100.0f;
        float modelScale = 1.0f; // box 1x1 in game units
        mapped[16] = modelScale; mapped[20] = 0.0f;       mapped[24] = 0.0f; mapped[28] = modelX;
        mapped[17] = 0.0f;       mapped[21] = modelScale; mapped[25] = 0.0f; mapped[29] = modelY;
        mapped[18] = 0.0f;       mapped[22] = 0.0f;       mapped[26] = 1.0f; mapped[30] = 0.0f;
        mapped[19] = 0.0f;       mapped[23] = 0.0f;       mapped[27] = 0.0f; mapped[31] = 1.0f;

        // ---- Model matrix for second box ----
        modelX = 90.0f + x;
        modelY = 95.0f + x / 2.0f;
        mapped[32] = modelScale; mapped[36] = 0.0f;       mapped[40] = 0.0f; mapped[44] = modelX;
        mapped[33] = 0.0f;       mapped[37] = modelScale; mapped[41] = 0.0f; mapped[45] = modelY;
        mapped[34] = 0.0f;       mapped[38] = 0.0f;       mapped[42] = 1.0f; mapped[46] = 0.0f;
        mapped[35] = 0.0f;       mapped[39] = 0.0f;       mapped[43] = 0.0f; mapped[47] = 1.0f;

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);

        SDL_PushGPUVertexUniformData(cmd, 0, mapped, 16 * sizeof(float));

        SDL_GPUTexture *swapchain;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain, &window_width, &window_height);

        SDL_GPUColorTargetInfo colors = {};
        colors.texture = swapchain;
        colors.load_op = SDL_GPU_LOADOP_CLEAR;
        colors.store_op = SDL_GPU_STOREOP_STORE;
        colors.clear_color = {0, 0, 0, 1};
        SDL_GPURenderPass *render = SDL_BeginGPURenderPass(cmd, &colors, 1, NULL);

        SDL_BindGPUGraphicsPipeline(render, pipeline);

        SDL_GPUViewport viewport = {};
        viewport.w = window_width;
        viewport.h = window_height;
        SDL_SetGPUViewport(render, &viewport);

        SDL_GPUBufferBinding binding = {};
        binding.buffer = gpu_buffer;
        SDL_BindGPUVertexBuffers(render, 0, &binding, 1);

        SDL_PushGPUVertexUniformData(cmd, 1, mapped + 16, 16 * sizeof(float));
        SDL_DrawGPUPrimitives(render, 4, 1, 0, 0);
        SDL_PushGPUVertexUniformData(cmd, 1, mapped + 32, 16 * sizeof(float));
        SDL_DrawGPUPrimitives(render, 4, 1, 0, 0);

        SDL_EndGPURenderPass(render);
        SDL_SubmitGPUCommandBuffer(cmd);


        x += 0.01f;
        // Do game logic, present a frame, etc.
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
