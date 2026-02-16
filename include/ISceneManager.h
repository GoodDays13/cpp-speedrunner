#pragma once

#include <memory>
#include <string>
#include <vector>

class IScene;

class ISceneManager {
public:
    virtual ~ISceneManager() = default;
    virtual void queuePushScene(std::unique_ptr<IScene> scene) = 0;
    virtual void queuePopScene(const IScene* requestingScene) = 0;
    virtual void queueSwitchToScene(std::unique_ptr<IScene> scene) = 0;

    virtual void startLevel(std::string levelName) = 0;

    void startLevel(int levelNum) { currentLevelIndex = levelNum; startLevel(levelOrder.at(levelNum)); }
    void startLevel() { startLevel(currentLevelIndex); }
    void nextLevel() { startLevel(currentLevelIndex + 1); }

    std::vector<std::string> levelOrder;
    int currentLevelIndex;
};
