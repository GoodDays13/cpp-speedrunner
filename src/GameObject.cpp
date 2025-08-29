#include "GameObject.h"
#include <cstdlib>

GameObject::GameObject() : position({}), scale({1, 1}), velocity({}) {}
GameObject::GameObject(Game* game) : GameObject() {
    this->game = game;
}

void GameObject::update(float deltaTime) {
    position += velocity * deltaTime;
}

void GameObject::handleEvent(const SDL_Event& event) {}

bool GameObject::isTouching(const GameObject& other) {
    float dx = std::abs(other.position.x - position.x);
    float px = (other.scale.x / 2 + scale.x / 2) - dx;
    if (px < 0) return false;

    float dy = std::abs(other.position.y - position.y);
    float py = (other.scale.y / 2 + scale.y / 2) - dy;
    if (py < 0) return false;

    return true;
}

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

void GameObject::handleMTV(Vector2 mtv) {};
