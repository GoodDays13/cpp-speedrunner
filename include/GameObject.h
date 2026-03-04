#pragma once

#include "IGameWorld.h"
#include "math.h"
#include "Video.h"
#include <SDL3/SDL_events.h>
#include <cstdint>

class GameObject {
public:
    GameObject();
    GameObject(IGameWorld* game);

    IGameWorld* game = nullptr;

    Transform transform;

    Vector4 color;

    uint32_t tags;

    virtual void update(float deltaTime);

    virtual void handleEvent(const SDL_Event& event);

    virtual void render(Video::RenderInfo& info);

    bool isTouching(const GameObject& other);

    Vector2 computeMTV(const GameObject& other);
};
