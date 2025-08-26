#pragma once

#include "math.h"
#include <SDL3/SDL_events.h>

class GameObject {
public:
    GameObject();

    Vector2 position;
    Vector2 scale;
    Vector2 velocity;

    virtual void update(float deltaTime);

    virtual void handleEvent(const SDL_Event& event);

    Vector2 computeMTV(const GameObject& other);
};

