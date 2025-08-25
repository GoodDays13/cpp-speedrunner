#include "game.h"
#include "video.h"
#include <vector>

Game::Game() = default;

bool Game::initialize() {
    if (!video.initialize())
        return false;

    return true;
};

void Game::run() {
    isRunning = true;
    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }

        update();
        render();
    }
}

void Game::update() {
}

void Game::render() {
    video.render({}, {}, std::vector<Video::RenderInfo>());
}
