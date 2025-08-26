#include "GameObject.h"
#include <cstdlib>

struct Collision {
    GameObject* other;
    Vector2 mtv; // Minimum Translation Vector
};

GameObject::GameObject() : position({}), scale({1, 1}), velocity({}) {}

void GameObject::update(float deltaTime) {
    position += velocity * deltaTime;
}

void GameObject::handleEvent(const SDL_Event& event) {}

Vector2 GameObject::computeMTV(const GameObject& other) {
    float dx = (other.position.x - position.x);
    float px = (other.scale.x / 2 + scale.x / 2) - std::abs(dx);
    if (px <= 0) return {0, 0};

    float dy = (other.position.y - position.y);
    float py = (other.scale.y / 2 + scale.y / 2) - std::abs(dy);
    if (py <= 0) return {0, 0};

    if (px < py) {
        return {dx < 0 ? px : -px, 0};
    } else {
        return {0, dy < 0 ? py : -py};
    }
}
