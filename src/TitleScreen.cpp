#include "FileReader.h"
#include "TitleScreen.h"
#include "PlatformerScene.h"
#include <SDL3/SDL_scancode.h>
#include <memory>

void TitleScreen::initialize(ISceneManager* sceneManager) {
    this->sceneManager = sceneManager;
}

void TitleScreen::handleEvent(SDL_Event event, const Video& video) {
    if (event.type == SDL_EVENT_KEY_UP) {
        switch (event.key.scancode) {
            case SDL_SCANCODE_RETURN: {
                auto jsonFileReader = FileReader("example.json");
                auto jsonValue = std::make_shared<JsonValue>(jsonFileReader.readJsonFile());
                auto platformerScene = std::make_unique<PlatformerScene>(jsonValue);
                sceneManager->queueSwitchToScene(std::move(platformerScene));
                break;
            } case SDL_SCANCODE_ESCAPE:
                sceneManager->queuePopScene(this);
                break;
            default:
                break;
        }
    }
}

void TitleScreen::update(float deltaTime) {
}

Video::RenderInfo TitleScreen::render() {
    Video::RenderInfo info;
    // Render a simple title screen (e.g., a colored background)
    Video::InstanceInfo titleText = {Video::QUAD, {{{0, 0}, {8, 4.5f}}, {1, 1, 1, 1}}}; // White rectangle as placeholder for title text
    info.instances.push_back(titleText);
    info.camera = {{0, 0}, {16, 9}};
    return info;
}
