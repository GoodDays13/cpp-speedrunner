#pragma once

#include "IGameWorld.h"
#include "math.h"
#include <SDL3/SDL_events.h>

class GameObject {
public:
    GameObject();
    GameObject(IGameWorld* game);

    IGameWorld* game = nullptr;

    Transform transform;
    Vector2 velocity;

    Vector4 color;

    virtual void update(float deltaTime);

    virtual void handleEvent(const SDL_Event& event);

    bool isTouching(const GameObject& other);

    Vector2 computeMTV(const GameObject& other);
    virtual void handleMTV(Vector2 mtv);
};
