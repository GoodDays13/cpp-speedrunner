#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Video.h"
#include <SDL3/SDL_stdinc.h>
#include <optional>
#include <vector>

class Game {
public:
    Game();
    bool initialize();
    void run();
    void cleanup();

    std::optional<Collision> checkCollisions(GameObject* obj);
private:
    Video video;
    std::vector<GameObject*> objects;
    Player* player = nullptr;
    Uint64 lastFrameTime = 0;
    Vector2 cameraPosition = {0, 0};
    float framerateLimit = 0.0f;
    float timeSpeed = 1.0f;

    void processInput();
    void update(float deltaTime);
    void render();
    bool isRunning;
};
