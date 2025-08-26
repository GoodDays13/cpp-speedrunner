#pragma once

#include "GameObject.h"
#include "Player.h"
#include "Video.h"
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
    unsigned long lastFrameTime;
    Vector2 cameraPosition;

    void processInput();
    void update(float deltaTime);
    void render();
    bool isRunning;
};
