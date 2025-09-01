#pragma once
#include "IScene.h"

class TitleScreen : public IScene {
public:
    TitleScreen() = default;
    ~TitleScreen() = default;

    void initialize(ISceneManager* sceneManager) override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;
};
