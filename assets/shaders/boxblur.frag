#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform sampler2D screenBuffer;
layout(set = 3, binding = 0) uniform Uniform {
    bool horizontal;
    int radius;
} u;

void main() {
    ivec2 size = textureSize(screenBuffer, 0);
    vec2 texelSize = 1.0 / size;
    vec4 pixel = vec4(0);
    float sigma = float(u.radius) / 3.0;
    float sum = 0;
    for (int x = -u.radius; x <= u.radius; x++) {
        vec2 offset = vec2(0.0);
        if (u.horizontal)
            offset.x = x;
        else
            offset.y = x;
        float weight = exp(-(x * x) / (2 * sigma * sigma));
        sum += weight;
        pixel += weight * texture(screenBuffer, uv + offset * texelSize);
    }
    pixel /= sum;
    fragColor = pixel;
}
