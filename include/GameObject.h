#pragma once

#include "math.h"
#include <SDL3/SDL_events.h>
#include <memory>

class Game;

class GameObject {
public:
    GameObject();
    GameObject(Game* game);

    Game* game = nullptr;

    Vector2 position;
    Vector2 scale;
    Vector2 velocity;

    virtual void update(float deltaTime);

    virtual void handleEvent(const SDL_Event& event);

    bool isTouching(const GameObject& other);

    Vector2 computeMTV(const GameObject& other);
    virtual void handleMTV(Vector2 mtv);
};

struct Collision {
    std::weak_ptr<GameObject> other;
    Vector2 normal;
    float time;
};
