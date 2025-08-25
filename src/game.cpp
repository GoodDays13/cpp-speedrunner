#include "game.h"
#include "video.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include <vector>

Game::Game() = default;

bool Game::initialize() {
    if (!video.initialize())
        return false;

    player = new GameObject();

    objects.push_back(player);

    return true;
};

void Game::run() {
    isRunning = true;
    while (isRunning) {
        unsigned long currentTime = SDL_GetTicks();
        float deltaTime = (float)(currentTime - lastFrameTime) / 1000;
        lastFrameTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                if (event.key.repeat)
                    continue;
                switch (event.key.scancode) {
                    case SDL_SCANCODE_A:
                        player->velocity.x += event.type == SDL_EVENT_KEY_DOWN ? -1 : 1;
                        break;
                    case SDL_SCANCODE_D:
                        player->velocity.x += event.type == SDL_EVENT_KEY_DOWN ? 1 : -1;
                        break;
                    default:
                        break;
                }
            }
        }

        update(deltaTime);
        render();
    }
}

void Game::update(float deltaTime) {
    for (int i = 0; i < objects.size(); i++) {
        GameObject* object = objects[i];
        object->position += object->velocity * deltaTime;
    }
}

void Game::render() {
    std::vector<Video::RenderInfo> infos;
    for (int i = 0; i < objects.size(); i++) {
        infos.push_back({Video::SQUARE, objects[i]->position, objects[i]->scale});
    }
    video.render({0, 0}, {16, 9}, infos);
}
