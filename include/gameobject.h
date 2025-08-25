#include "video.h"
class GameObject {
public:
    GameObject() : position({}), scale({1, 1}), velocity({}) {}

    Vector2 position;
    Vector2 scale;
    Vector2 velocity;
};
