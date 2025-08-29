#pragma once

#include "GameObject.h"
#include <SDL3/SDL_events.h>

class Player : public GameObject {
private:
    const float speed = 5.0f;
    const float jumpSpeed = 5.0f;
    const float gravity = 24.0f;
    const float accelerationTime = 0.1f;
    const float terminalVelocity = 8.0f;
    const float coyoteTime = 0.1f;
    float coyoteTimer = 0.0f;
    const float jumpTime = 0.4f;
    float jumpTimer = 0.0f;
    Vector2 input = {0, 0};
public:
    Player();
    Player(Game* game) : GameObject(game) {}

    void handleEvent(const SDL_Event& event) override;
    void update(float deltaTime) override;
    void handleMTV(Vector2 mtv) override;
};
