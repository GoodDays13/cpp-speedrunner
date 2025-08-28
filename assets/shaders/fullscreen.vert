#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 fragUV;

void main() {
    vec4 pos = vec4(position.x * 2, position.y * 2, 0.0, 1.0);
    gl_Position = pos;
    fragUV = uv;
}
