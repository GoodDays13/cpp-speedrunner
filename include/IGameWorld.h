#pragma once

#include "math.h"
#include <memory>
#include <optional>
#include <vector>

class GameObject;

struct Collision {
    std::weak_ptr<GameObject> other;
    Vector2 normal;
    float time;
};

class IGameWorld {
public:
    virtual ~IGameWorld() = default;

    virtual void completeLevel() = 0;

    virtual std::optional<Collision> checkCollisions(const GameObject& obj) = 0;
    virtual std::vector<std::weak_ptr<GameObject>> findObjectsAtCoords(Vector2 pos) = 0;
    virtual std::weak_ptr<GameObject> createGameObject() = 0;
    virtual void destroyGameObject(const std::weak_ptr<GameObject> obj) = 0;
};
