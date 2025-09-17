#pragma once

#include <memory>

class IScene;

class ISceneManager {
public:
    virtual ~ISceneManager() = default;
    virtual void queuePushScene(std::unique_ptr<IScene> scene) = 0;
    virtual void queuePopScene(const IScene* requestingScene) = 0;
    virtual void queueSwitchToScene(std::unique_ptr<IScene> scene) = 0;

    virtual void startLevel(std::string levelName) = 0;
};
