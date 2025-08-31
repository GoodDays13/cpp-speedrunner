#pragma once

#include "IScene.h"
#include "Video.h"
#include <memory>

class Game {
private:
    std::unique_ptr<Video> video;
    std::vector<std::unique_ptr<IScene>> sceneStack;
    Uint64 lastFrameTime = 0;
    float framerateLimit = 0.0f;
    bool isRunning;
public:
    bool initialize();
    void run();
    void cleanup();

    void pushScene(std::unique_ptr<IScene> scene);
    void popScene();
    void switchToScene(std::unique_ptr<IScene> scene);
};
