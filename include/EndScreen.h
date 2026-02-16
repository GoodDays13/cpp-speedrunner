#pragma once
#include "IScene.h"

struct EndScreenData {
    Uint64 completionTimeMS;
};

class EndScreen : public IScene {
private:
    EndScreenData data;
public:
    EndScreen(EndScreenData data) : data(data) {}

    void initialize(ISceneManager* sceneManager) override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;
};
