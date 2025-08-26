#include "Player.h"

Player::Player() = default;

void Player::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        if (event.key.repeat)
            return;
        switch (event.key.scancode) {
            case SDL_SCANCODE_A:
                velocity.x += event.type == SDL_EVENT_KEY_DOWN ? -speed : speed;
                break;
            case SDL_SCANCODE_D:
                velocity.x += event.type == SDL_EVENT_KEY_DOWN ? speed : -speed;
                break;
            case SDL_SCANCODE_SPACE:
                if (event.type == SDL_EVENT_KEY_DOWN && coyoteTimer > 0.0f) {
                    velocity.y = speed * 1.5f;
                    coyoteTimer = 0.0f;
                }
                break;
            default:
                break;
        }
    }
}

void Player::update(float deltaTime) {
    coyoteTimer -= deltaTime;
    velocity.y -= gravity * deltaTime;
    GameObject::update(deltaTime);
}
