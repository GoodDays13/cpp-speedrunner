#pragma once

#include "math.h"
#include "Video.h"
#include <string>
#include <vector>

struct Font {
    std::string path;
    float fontSize = 1;
};

class Text {
public:
    enum class Align {
        Left,
        Center,
        Right,
    };

    Text(std::string text, Transform transform, Font font, Align align) : text(text), transform(transform), font(font), align(align) {}

    std::string getText();
    Transform getTransform();

    void setText(std::string text);
    void setTransform(Transform transform);

    std::vector<Video::MiscData> getData();
    Video::RenderKey getKey();
private:
    Font font;
    std::string text;
    // position is top-left corner
    Transform transform;

    Align align;

    std::vector<Video::MiscData> cached;
};
