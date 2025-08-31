#pragma once

#include "GameObject.h"
#include "IGameWorld.h"
#include "IScene.h"
#include "Player.h"
#include "Video.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <memory>
#include <optional>
#include <vector>

class PlatformerScene : public IScene, public IGameWorld {
public:
    PlatformerScene() = default;
    ~PlatformerScene() = default;

    void initialize() override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;

    std::optional<Collision> checkCollisions(const GameObject& obj) override;
    std::vector<std::weak_ptr<GameObject>> findObjectsAtCoords(Vector2 pos) override;
    std::weak_ptr<GameObject> createGameObject() override;
    void destroyGameObject(const std::weak_ptr<GameObject> obj) override;
private:
    std::vector<std::shared_ptr<GameObject>> objects;
    std::weak_ptr<Player> player;
    Vector2 cameraPosition = {0, 0};
    float timeSpeed = 1.0f;
};
