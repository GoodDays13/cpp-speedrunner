#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec2 model_position;
layout(location = 3) in vec2 model_scale;
layout(location = 4) in vec4 color;
layout(location = 5) in uint index;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

layout(set = 1, binding = 0) uniform Camera {
    mat4 view;
} camera;

layout(set = 1, binding = 1) uniform Sprite {
    uint width;
    uint height;
} sprite;

void main() {
    mat4 model = mat4(
            vec4(model_scale.x, 0.0, 0.0, 0.0),
            vec4(0.0, model_scale.y, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(model_position.x, model_position.y, 0.0, 1.0)
        );
    vec4 pos = vec4(position, 0.0, 1.0);
    pos = camera.view * model * pos;
    gl_Position = pos;
    fragUV = (uv + vec2(index % sprite.width, index / sprite.width)) / vec2(sprite.width, sprite.height);
    fragColor = color;
}
