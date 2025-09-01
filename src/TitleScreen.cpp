#include "TitleScreen.h"
#include "PlatformerScene.h"
#include <memory>

void TitleScreen::initialize(ISceneManager* sceneManager) {
    this->sceneManager = sceneManager;
}

void TitleScreen::handleEvent(SDL_Event event, const Video& video) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        // On any key press, exit the title screen (in a real game, you'd probably want to switch to another scene)
        // For this example, we'll just log a message
        SDL_Log("Key pressed, exiting title screen.");
        sceneManager->queueSwitchToScene(std::make_unique<PlatformerScene>());
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
