#pragma once

#include "GameObject.h"
#include <SDL3/SDL_events.h>

class Player : public GameObject {
public:
    Player();
    float speed = 5.0f;
    float gravity = 7.0f;
    const float coyoteTime = 0.1f;
    float coyoteTimer = 0.0f;

    void handleEvent(const SDL_Event& event) override;
    void update(float deltaTime) override;
};
