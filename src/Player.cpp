#include "Player.h"
#include "Game.h"
#include <cmath>
#include <optional>

Player::Player() = default;

void Player::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        if (event.key.repeat)
            return;
        switch (event.key.scancode) {
            case SDL_SCANCODE_A:
                input.x += event.type == SDL_EVENT_KEY_DOWN ? -1 : 1;
                break;
            case SDL_SCANCODE_D:
                input.x += event.type == SDL_EVENT_KEY_DOWN ? 1 : -1;
                break;
            case SDL_SCANCODE_SPACE:
                if (event.type == SDL_EVENT_KEY_DOWN && coyoteTimer > 0.0f) {
                    coyoteTimer = 0.0f;
                    jumpTimer = jumpTime;
                } else if (event.type == SDL_EVENT_KEY_UP) {
                    jumpTimer = 0.0f;
                }
                break;
            default:
                break;
        }
    }
}

void Player::update(float deltaTime) {
    velocity.y = std::max(velocity.y - gravity * deltaTime, -terminalVelocity);
    if (jumpTimer > 0.0f) {
        velocity.y = jumpSpeed;
        jumpTimer -= deltaTime;
    }
    coyoteTimer -= deltaTime;

    float desiredDifference = velocity.x - input.x * speed;
    float accel = (coyoteTimer > 0 ? 1.0f : 0.75f) / accelerationTime * speed * deltaTime;
    if (std::fabs(desiredDifference) < accel) {
        velocity.x = input.x * speed;
    } else {
        velocity.x += accel * (desiredDifference > 0 ? -1 : 1);
    }

    for (int i = 0; i < touching.size(); i++) {
        if (!isTouching(*touching[i].other)) {
            touching.erase(touching.begin() + i);
            i--;
            continue;
        }
        if (touching[i].normal.y > 0) {
            coyoteTimer = coyoteTime;
            velocity.y = std::max(velocity.y, 0.0f);
        } else if (touching[i].normal.y < 0) {
            velocity.y = std::min(velocity.y, 0.0f);
        }
        if (touching[i].normal.x > 0) {
            velocity.x = std::max(velocity.x, 0.0f);
        } else if (touching[i].normal.x < 0) {
            velocity.x = std::min(velocity.x, 0.0f);
        }
    }

    std::optional<Collision> collision = std::nullopt;
    if (game) {
        collision = game->checkCollisions(this);
    }
    if (collision && collision->time < deltaTime) {
        position += velocity * collision->time;
        if (collision->normal.y > 0) {
            coyoteTimer = coyoteTime;
            velocity.y = std::max(velocity.y, 0.0f);
        } else if (collision->normal.y < 0) {
            velocity.y = std::min(velocity.y, 0.0f);
            jumpTimer = 0.0f;
        }
        if (collision->normal.x != 0) {
            velocity.x = 0.0f;
        }
        touching.push_back(*collision);
    } else {
        position += velocity * deltaTime;
    }

    if (position.y < -20.0f) {
        position = {0.0f, 0.0f};
        velocity.y = 0.0f;
    }
}

void Player::handleMTV(Vector2 mtv) {
    position += mtv;
    if (mtv.y > 0) {
        coyoteTimer = coyoteTime;
        velocity.y = std::max(velocity.y, 0.0f);
    } else if (mtv.y < 0) {
        velocity.y = std::min(velocity.y, 0.0f);
        jumpTimer = 0.0f;
    }
    if (mtv.x != 0) {
        velocity.x = 0.0f;
    }
}
