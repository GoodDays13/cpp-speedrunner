#pragma once

#include "IScene.h"
#include "ISceneManager.h"
#include "Video.h"
#include <memory>

class Game : public ISceneManager {
private:
    std::unique_ptr<Video> video;
    std::vector<std::unique_ptr<IScene>> sceneStack;
    Uint64 lastFrameTime = 0;
    float framerateLimit = 0.0f;
    bool isRunning;

    void pushScene(std::unique_ptr<IScene> scene);
    void popScene();
    void switchToScene(std::unique_ptr<IScene> scene);
    std::vector<std::unique_ptr<IScene>> pendingPushScenes;
    bool pendingPopScene = false;
    std::unique_ptr<IScene> pendingSwitchScene = nullptr;
public:
    bool initialize();
    void run();
    void cleanup();

    void queuePushScene(std::unique_ptr<IScene> scene) override;
    void queuePopScene(const IScene* requestingScene) override;
    void queueSwitchToScene(std::unique_ptr<IScene> scene) override;
    void startLevel(std::string levelName) override;
};
