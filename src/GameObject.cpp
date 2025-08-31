#include "GameObject.h"
#include <cstdlib>

GameObject::GameObject() : transform({{}, {1, 1}}), velocity({}), color({1, 1, 1, 1}) {}
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
    float dx = (other.transform.position.x - transform.position.x);
    float px = (other.transform.scale.x / 2 + transform.scale.x / 2) - std::abs(dx);
    if (px <= 0) return {0, 0};

    float dy = (other.transform.position.y - transform.position.y);
    float py = (other.transform.scale.y / 2 + transform.scale.y / 2) - std::abs(dy);
    if (py <= 0) return {0, 0};

    if (px < py) {
        return {dx < 0 ? px : -px, 0};
    } else {
        return {0, dy < 0 ? py : -py};
    }
}

void GameObject::handleMTV(Vector2 mtv) {};
