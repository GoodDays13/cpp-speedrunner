#pragma once

#include "ISceneManager.h"
#include "Video.h"
#include <SDL3/SDL_events.h>

class IScene {
protected:
    ISceneManager* sceneManager = nullptr;
public:
    virtual ~IScene() = default;

    virtual void initialize(ISceneManager* sceneManager) = 0;
    virtual void handleEvent(SDL_Event event, const Video& video) = 0;
    virtual void update(float deltaTime) = 0;
    virtual Video::RenderInfo render() = 0;
};
