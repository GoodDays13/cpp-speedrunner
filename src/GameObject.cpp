#include "GameObject.h"
#include <SDL3/SDL_log.h>
#include <cstdlib>

GameObject::GameObject() : transform({{}, {1, 1}}), velocity({}), color({1, 1, 1, 1}), tags(0) {}
GameObject::GameObject(IGameWorld* game) : GameObject() {
    this->game = game;
}

void GameObject::update(float deltaTime) {
}

void GameObject::handleEvent(const SDL_Event& event) {}

bool GameObject::isTouching(const GameObject& other) {
    float dx = std::abs(other.transform.position.x - transform.position.x);
    float px = (other.transform.scale.x / 2 + transform.scale.x / 2) - dx;
    if (px < 0) return false;

    float dy = std::abs(other.transform.position.y - transform.position.y);
    float py = (other.transform.scale.y / 2 + transform.scale.y / 2) - dy;
    if (py < 0) return false;

    return true;
}

Vector2 GameObject::computeMTV(const GameObject& other) {
    float left = other.transform.position.x - other.transform.scale.x / 2 - transform.scale.x / 2;
    float right = other.transform.position.x + other.transform.scale.x / 2 + transform.scale.x / 2;
    float bottom = other.transform.position.y - other.transform.scale.y / 2 - transform.scale.y / 2;
    float top = other.transform.position.y + other.transform.scale.y / 2 + transform.scale.y / 2;

    float leftOverlap = right - transform.position.x;
    float rightOverlap = left - transform.position.x;
    if (leftOverlap < 0 || rightOverlap > 0) {
        // No overlap
        return {0, 0};
    }

    float bottomOverlap = top - transform.position.y;
    float topOverlap = bottom - transform.position.y;
    if (bottomOverlap < 0 || topOverlap > 0) {
        // No overlap
        return {0, 0};
    }

    float xOverlap = std::abs(leftOverlap) < std::abs(rightOverlap) ? leftOverlap : rightOverlap;
    float yOverlap = std::abs(bottomOverlap) < std::abs(topOverlap) ? bottomOverlap : topOverlap;

    if (std::abs(xOverlap) < std::abs(yOverlap)) {
        return {xOverlap, 0};
    } else {
        return {0, yOverlap};
    }
}
