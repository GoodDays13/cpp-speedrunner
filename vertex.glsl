#version 450

layout(location = 0) in vec2 position;

layout(set = 1, binding = 0) uniform Camera {
    mat4 view;       // View matrix for camera transformations
} camera;

layout(set = 1, binding = 1) uniform Matrices {
    mat4 model;      // Model matrix for object transformations
} uniforms;

void main() {
    vec4 pos = vec4(position, 0.0, 1.0);
    pos = camera.view * uniforms.model * pos;
    gl_Position = pos;
}
