#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;
layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform sampler2D screenBuffer;

void main() {
    fragColor = color * texture(screenBuffer, uv);
}
