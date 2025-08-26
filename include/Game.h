#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Video.h"
#include <SDL3/SDL_stdinc.h>
#include <vector>

class Game {
public:
    Game();
    bool initialize();
    void run();
    void cleanup();
private:
    Video video;
    std::vector<GameObject*> objects;
    Player* player;
    Uint64 lastFrameTime;
    Vector2 cameraPosition;
    float framerateLimit;

    void processInput();
    void update(float deltaTime);
    void render();
    bool isRunning;
};
