#include "EndScreen.h"
#include "Text.h"
#include "TitleScreen.h"
#include <SDL3/SDL_events.h>
#include <format>
#include <memory>

void EndScreen::initialize(ISceneManager* sceneManager) {
    this->sceneManager = sceneManager;
}

void EndScreen::handleEvent(SDL_Event event, const Video& video) {
    if (event.type == SDL_EVENT_KEY_UP) {
        if (sceneManager->currentLevelIndex == sceneManager->levelOrder.size() - 1) {
            sceneManager->queueSwitchToScene(std::make_unique<TitleScreen>());
            sceneManager->currentLevelIndex = 0;
        } else
            sceneManager->nextLevel();
    }
}

void EndScreen::update(float deltaTime) {
}

Video::RenderInfo EndScreen::render() {
    Video::RenderInfo info;

    float completionTimeSec = static_cast<float>(data.completionTimeMS) / 1000.0f;

    Text label("Time:", {{0, 1}, {1, 1}}, {"fonts/x05mo.png"}, Text::Align::Center);
    Text time(std::format("{:.3f}", completionTimeSec), {{0, 0}, {5, 1}}, {"fonts/x05mo.png"}, Text::Align::Center);

    info.renderBatches[label.getKey()].append_range(label.getData());
    info.renderBatches[time.getKey()].append_range(time.getData());

    info.camera = {{0, 0}, {16, 9}};
    return info;
}
