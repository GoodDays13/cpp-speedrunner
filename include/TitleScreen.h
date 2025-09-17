#pragma once
#include "GameObject.h"
#include "IScene.h"
#include <functional>
#include <utility>

class TitleScreen : public IScene {
private:
    GameObject title;
    std::vector<std::pair<std::function<void()>, GameObject>> menuItems;
    size_t selectedItem = 0;
public:
    void initialize(ISceneManager* sceneManager) override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;
};
