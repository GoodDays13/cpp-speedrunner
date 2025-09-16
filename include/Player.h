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
    Vector2 startPosition = {0, 0};
public:
    Player() = default;
    Player(IGameWorld* game) : GameObject(game) {}
    Player(IGameWorld* game, Vector2 pos) : GameObject(game) { transform.position = pos; startPosition = pos; }

    void handleEvent(const SDL_Event& event) override;
    void update(float deltaTime) override;

    void setStartPosition(Vector2 pos);
    void respawn();
};
