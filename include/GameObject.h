#pragma once

#include "IGameWorld.h"
#include "math.h"
#include <SDL3/SDL_events.h>
#include <algorithm>

class GameObject {
private:
    std::vector<std::string> tags;
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

    bool hasTag(const std::string& tag) const {
        return std::find(tags.begin(), tags.end(), tag) != tags.end();
    }

    void addTag(const std::string& tag) {
        if (!hasTag(tag)) tags.push_back(tag);
    }

    void removeTag(const std::string& tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    }
};
