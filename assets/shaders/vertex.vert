#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 model_position;
layout(location = 2) in vec2 model_scale;

layout(set = 1, binding = 0) uniform Camera {
    mat4 view;
} camera;

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
}
