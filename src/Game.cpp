#include "Game.h"
#include "GameObject.h"
#include "Video.h"
#include "Player.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <cmath>
#include <optional>
#include <vector>

Game::Game() = default;

bool Game::initialize() {
    if (!video.initialize())
        return false;

    framerateLimit = video.getDisplayMode()->refresh_rate;
    timeSpeed = 1.0f;

    player = new Player(this);

    objects.push_back(player);

    GameObject* floor = new GameObject(this);
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
        deltaTime *= timeSpeed;

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
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_KP_PLUS) {
                    timeSpeed *= 1.25f;
                } else if (event.key.scancode == SDL_SCANCODE_KP_MINUS) {
                    timeSpeed *= 0.8f;
                }
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

    cameraPosition += (player->position - cameraPosition) * 5.0f * deltaTime;
}

void Game::render() {
    std::vector<Video::RenderInfo> infos;
    for (int i = 0; i < objects.size(); i++) {
        infos.push_back({Video::SQUARE, objects[i]->position, objects[i]->scale});
    }
    video.render(cameraPosition, {16, 9}, infos);
}

std::optional<Collision> Game::checkCollisions(GameObject* obj) {
    if (obj->velocity.x == 0.0f && obj->velocity.y == 0.0f)
        return std::nullopt;
    std::vector<Collision> collisions;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i] == obj)
            continue;
        GameObject* other = objects[i];

        float left = other->position.x - other->scale.x / 2 - obj->scale.x / 2;
        float right = other->position.x + other->scale.x / 2 + obj->scale.x / 2;
        float bottom = other->position.y - other->scale.y / 2 - obj->scale.y / 2;
        float top = other->position.y + other->scale.y / 2 + obj->scale.y / 2;

        if (obj->velocity.x == 0.0f) {
            if (obj->position.x <= left || obj->position.x >= right) {
                continue;
            }
        } else if (obj->velocity.x > 0.0f) {
            if (obj->position.x >= right)
                continue;
        } else {
            if (obj->position.x <= left)
                continue;
        }
        if (obj->velocity.y == 0.0f) {
            if (obj->position.y <= bottom || obj->position.y >= top)
                continue;
        } else if (obj->velocity.y > 0.0f) {
            if (obj->position.y >= top)
                continue;
        } else {
            if (obj->position.y <= bottom)
                continue;
        }

        float xEntry = ((obj->velocity.x >= 0 ? left : right) - obj->position.x) / obj->velocity.x;
        float xExit =  ((obj->velocity.x >= 0 ? right : left) - obj->position.x) / obj->velocity.x;
        float yEntry = ((obj->velocity.y >= 0 ? bottom : top) - obj->position.y) / obj->velocity.y;
        float yExit =  ((obj->velocity.y >= 0 ? top : bottom) - obj->position.y) / obj->velocity.y;


        if (xEntry > yExit || yEntry > xExit)
            continue;

        if (xEntry < 0.0f && yEntry < 0.0f)
            continue;

        Collision collision = {};
        collision.other = other;
        if (xEntry > yEntry) {
            collision.time = xEntry;
            collision.normal = obj->velocity.x > 0 ? Vector2(-1, 0) : Vector2(1, 0);
        } else {
            collision.time = yEntry;
            collision.normal = obj->velocity.y > 0 ? Vector2(0, -1) : Vector2(0, 1);
        }
        collisions.push_back(collision);
    }

    if (collisions.size() == 0)
        return std::nullopt;

    int min = 0;
    for (int i = 1; i < collisions.size(); i++)
        if (collisions[i].time < collisions[min].time) min = i;


    return collisions[min];
}
