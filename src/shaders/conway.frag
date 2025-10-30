#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform sampler2D screenBuffer;

void main() {
    ivec2 size = textureSize(screenBuffer, 0);
    vec2 texelSize = 1.0 / size;
    vec4 original = texture(screenBuffer, uv);
    vec4 colorSum = vec4(0);

    int neighbors = 0;
    bool alive;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 offset = vec2(x, y) * texelSize;
            bool neighborAlive = false;
            vec2 location = uv + offset;
            if (location.x >= 0.0 && location.y >= 0.0 && location.x < 1.0 && location.y < 1.0) {
                vec4 pixel = texture(screenBuffer, uv + offset);
                neighborAlive = pixel.x + pixel.y + pixel.z > 0.0;
                if (neighborAlive) colorSum += pixel;
            }
            if (x == 0 && y == 0) {
                alive = neighborAlive;
            } else if (neighborAlive) {
                neighbors++;
            }
        }
    }
    bool newAliveStatus = (neighbors == 3) || (neighbors == 2 && alive);
    vec4 newColor = alive ? original : (colorSum / float(neighbors));
    fragColor = newAliveStatus ? newColor : vec4(0, 0, 0, 1);
}
