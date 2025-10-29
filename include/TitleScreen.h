#pragma once
#include "GameObject.h"
#include "IScene.h"
#include "Text.h"
#include <functional>

class TitleScreen : public IScene {
private:
    struct MenuItem {
        std::function<void()> callback;
        GameObject object;
        Text text;
    };
    GameObject title;
    std::vector<MenuItem> menuItems;
    size_t selectedItem = 0;
public:
    void initialize(ISceneManager* sceneManager) override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;
};
