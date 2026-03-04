#include "Text.h"
#include "Video.h"
#include <vector>


std::string Text::getText() {
    return text;
}
Transform Text::getTransform() {
    return transform;
}

void Text::setText(std::string text) {
    this->text = text;
    cached = {};
}
void Text::setTransform(Transform transform) {
    this->transform = transform;
    cached = {};
}

void Text::render(Video::RenderInfo& info) {
    for (Video::MiscData letter : getData())
        info.drawSprite(letter.transform, font.path, letter.index);
}

const std::vector<Video::MiscData>& Text::getData() {
    if (!cached.empty()) return cached;
    std::vector<Video::MiscData> data;
    for (int i = 0; i < text.length(); i++) {
        float pos;
        switch (align) {
            case Align::Center:
                pos = transform.position.x + static_cast<float>(text.length()) / -2.0f + 0.5f + i;
                break;
            case Align::Left:
                pos = transform.position.x + i + 0.5f;
                break;
            case Align::Right:
                pos = transform.position.x - text.length() + 0.5f + i;
                break;
        }
        data.push_back({
            .transform = {{pos, transform.position.y}, {1, 1}},
            .index = static_cast<unsigned int>(text[i] - ' ')
        });
    }
    cached = data;
    return cached;
}
