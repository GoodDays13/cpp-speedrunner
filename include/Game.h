#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Video.h"
#include <SDL3/SDL_stdinc.h>
#include <memory>
#include <optional>
#include <vector>

class Game {
public:
    Game();
    bool initialize();
    void run();
    void cleanup();

    std::optional<Collision> checkCollisions(const GameObject& obj);
    std::vector<std::weak_ptr<GameObject>> findObjectsAtCoords(Vector2 pos);
    std::weak_ptr<GameObject> createGameObject();
    void destroyGameObject(const std::weak_ptr<GameObject> obj);
private:
    Video video;
    std::vector<std::shared_ptr<GameObject>> objects;
    std::weak_ptr<Player> player;
    Uint64 lastFrameTime = 0;
    Vector2 cameraPosition = {0, 0};
    float framerateLimit = 0.0f;
    float timeSpeed = 1.0f;

    void processInput();
    void update(float deltaTime);
    void render();
    bool isRunning;
};
