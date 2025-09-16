#include "Player.h"
#include "IGameWorld.h"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cmath>
#include <memory>
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
        input.x = std::clamp(input.x, -1.0f, 1.0f);
    }
}

void Player::update(float deltaTime) {
    // Gravity & terminal velocity
    velocity.y = std::max(velocity.y - gravity * deltaTime, -terminalVelocity);

    // Jumping
    if (jumpTimer > 0.0f) {
        velocity.y = jumpSpeed;
    }

    // Horizontal movement with acceleration
    float targetVelocityX = input.x * speed;
    float velocityDifference = velocity.x - targetVelocityX;
    float accelerationMultiplier = (coyoteTimer > 0) ? 1.0f : 0.75f; // coyoteTimer > 0 means on ground
    float maxAcceleration = accelerationMultiplier * speed / accelerationTime * deltaTime;

    if (std::fabs(velocityDifference) < maxAcceleration) {
        velocity.x = targetVelocityX; // Snap to target if within acceleration range
    } else {
        velocity.x += maxAcceleration * ((velocityDifference > 0) ? -1 : 1); // Move towards target
    }

    // Keep track of if the movement this frame kills us or completes the level.
    bool levelComplete = false;
    bool dead = false;

    // Keep track of remaining time to handle multiple collisions in one frame
    float remainingTime = deltaTime;
    while (remainingTime > 0.0f) {
        std::optional<Collision> collision = std::nullopt;
        std::shared_ptr<GameObject> other;
        if (game) { // Sanity check
            collision = game->checkCollisions(*this);
        }
        if (collision && (other = collision->other.lock()) && collision->time < remainingTime) {
            if (other->hasTag("end")) {
                levelComplete = true;
            } if (other->hasTag("kill")) {
                dead = true;
            }

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

            // Check if touching the end
            Vector2 mtv = computeMTV(*other);
            transform.position += mtv;
        } else {
            // No collision, move the full remaining time
            transform.position += velocity * remainingTime;
            remainingTime = 0.0f;
        }
    }

    // If killed during the frame, respawn. If complete level, do proper steps.
    // If killed and complete level at same time, death takes priority
    if (dead) respawn();
    else if (levelComplete) game->completeLevel();

    // Update timers
    jumpTimer -= deltaTime;
    coyoteTimer -= deltaTime;
}

void Player::setStartPosition(Vector2 pos) {
    startPosition = pos;
}

void Player::respawn() {
    transform.position = startPosition;
    velocity = {0, 0};
    coyoteTimer = 0.0f;
    jumpTimer = 0.0f;
}
