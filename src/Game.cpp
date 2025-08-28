#include "Game.h"
#include "Video.h"
#include "Player.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <vector>

Game::Game() = default;

bool Game::initialize() {
    if (!video.initialize())
        return false;

    framerateLimit = video.getDisplayMode()->refresh_rate;

    player = new Player();

    objects.push_back(player);

    GameObject* floor = new GameObject();
    floor->position = {-8, -4};
    floor->scale = {16, 1};
    objects.push_back(floor);

    return true;
};

void Game::run() {
    const Uint64 frameDelay = framerateLimit > 0 ? 1000000000ULL / framerateLimit : 0;
    lastFrameTime = SDL_GetTicksNS();
    isRunning = true;
    while (isRunning) {
        Uint64 currentTime = SDL_GetTicksNS();
        Uint64 elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime < frameDelay) {
            SDL_DelayNS(frameDelay - elapsedTime);
            currentTime = SDL_GetTicksNS();
            elapsedTime = currentTime - lastFrameTime;
        }
        float deltaTime = (float)elapsedTime / 1e9f;
        lastFrameTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float x = (event.button.x / video.window_width - 0.5) * 16 + cameraPosition.x;
                float y = (event.button.y / video.window_height - 0.5) * -9 + cameraPosition.y;
                GameObject* obj = new GameObject();
                obj->position = {x, y};
                objects.push_back(obj);
            }
            for (int i = 0; i < objects.size(); i++) {
                objects[i]->handleEvent(event);
            }
        }

        update(deltaTime);
        render();
    }
}

void Game::update(float deltaTime) {
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->update(deltaTime);
    }

    for (int i = 0; i < objects.size(); i++) {
        if (objects[i] == player)
            continue;
        Vector2 mtv = player->computeMTV(*objects[i]);
        player->position += mtv;
        if (mtv.y > 0) {
            player->coyoteTimer = player->coyoteTime;
            player->velocity.y = std::max(player->velocity.y, 0.0f);
        } else if (mtv.y < 0) {
            player->velocity.y = std::min(player->velocity.y, 0.0f);
        }
    }

    cameraPosition += (player->position - cameraPosition) * 5.0f * deltaTime;
}

void Game::render() {
    std::vector<Video::RenderInfo> infos;
    for (int i = 0; i < objects.size(); i++) {
        infos.push_back({Video::SQUARE, objects[i]->position, objects[i]->scale});
    }
    video.render(cameraPosition, {16, 9}, infos);
}
