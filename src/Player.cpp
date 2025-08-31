#include "Player.h"
#include "IGameWorld.h"
#include <SDL3/SDL_log.h>
#include <cmath>
#include <optional>

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


    // Keep track of remaining time to handle multiple collisions in one frame
    float remainingTime = deltaTime;
    while (remainingTime > 0.0f) {
        std::optional<Collision> collision = std::nullopt;
        if (game) { // Sanity check
            collision = game->checkCollisions(*this);
        }
        if (collision && collision->time < remainingTime) {
            // Move up to the collision
            transform.position += velocity * collision->time;
            remainingTime -= collision->time;

            // Reflect velocity based on collision normal
            if (collision->normal.y < 0) {
                jumpTimer = 0.0f; // Cancel jump if hitting head
            } else if (collision->normal.y > 0) {
                coyoteTimer = coyoteTime; // Reset coyote timer when landing
            }
            // Remove component of velocity in direction of normal
            velocity -= collision->normal * collision->normal.dot(velocity);

            // Handle any overlap due to floating point precision issues
            const float padding = 1e-6f;
            if (fabs(transform.position.x) < padding) {
                transform.position.x = 0.0f;
            } if (fabs(transform.position.y) < padding) {
                transform.position.y = 0.0f;
            }
        } else {
            // No collision, move the full remaining time
            transform.position += velocity * remainingTime;
            remainingTime = 0.0f;
        }
    }

    if (transform.position.y < -20.0f) {
        transform.position = {0.0f, 0.0f};
        velocity.y = 0.0f;
    }
}
