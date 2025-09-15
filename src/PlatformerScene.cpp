#include "PlatformerScene.h"
#include "GameObject.h"
#include "TitleScreen.h"
#include "Video.h"
#include "Player.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <optional>
#include <vector>

void PlatformerScene::initialize(ISceneManager* sceneManager) {
    this->sceneManager = sceneManager;
    timeSpeed = 1.0f;

    loadLevel();

    setupBinds();

    startMS = SDL_GetTicks();
};

void PlatformerScene::loadLevel() {
    JsonValue playerData = levelData["player"];
    JsonValue objectsData = levelData["objects"];

    camera = {{0, 0}, {16, 9}};

    auto playerPtr = std::make_shared<Player>(this);
    playerPtr->transform.position = {
        static_cast<float>(playerData["position"]["x"].getDouble()),
        static_cast<float>(playerData["position"]["y"].getDouble())
    };
    playerPtr->transform.scale = {
        static_cast<float>(playerData["scale"]["x"].getDouble()),
        static_cast<float>(playerData["scale"]["y"].getDouble())
    };
    playerPtr->color = {
        static_cast<float>(playerData["color"]["r"].getDouble()),
        static_cast<float>(playerData["color"]["g"].getDouble()),
        static_cast<float>(playerData["color"]["b"].getDouble()),
        static_cast<float>(playerData["color"]["a"].getDouble())
    };
    playerPtr->addTag("player");

    camera.position = playerPtr->transform.position;

    player = playerPtr;

    objects.push_back(playerPtr);

    for (int i = 0; i < objectsData.size(); i++) {
        JsonValue objData = objectsData[i];
        auto obj = createGameObject().lock();
        obj->transform.position = {
            static_cast<float>(objData["position"]["x"].getDouble()),
            static_cast<float>(objData["position"]["y"].getDouble())
        };
        obj->transform.scale = {
            static_cast<float>(objData["scale"]["x"].getDouble()),
            static_cast<float>(objData["scale"]["y"].getDouble())
        };
        obj->color = {
            static_cast<float>(objData["color"]["r"].getDouble()),
            static_cast<float>(objData["color"]["g"].getDouble()),
            static_cast<float>(objData["color"]["b"].getDouble()),
            static_cast<float>(objData["color"]["a"].getDouble())
        };
        for (int j = 0; j < objData["tags"].size(); j++) {
            obj->addTag(objData["tags"][j].getString());
        }
    }
}

void PlatformerScene::setupBinds() {
    keyActions[SDL_SCANCODE_ESCAPE] = [&](const SDL_Event& event) {
        if (event.type == SDL_EVENT_KEY_UP) sceneManager->queueSwitchToScene(std::make_unique<TitleScreen>());
    };
    keyActions[SDL_SCANCODE_KP_PLUS] = [&](const SDL_Event& event) {
        if (event.type == SDL_EVENT_KEY_DOWN) timeSpeed *= 1.25f;
    };
    keyActions[SDL_SCANCODE_KP_MINUS] = [&](const SDL_Event& event) {
        if (event.type == SDL_EVENT_KEY_DOWN) timeSpeed *= 0.8f;
    };
}

void PlatformerScene::handleEvent(SDL_Event event, const Video& video) {
    switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            // Create or remove a tile at a position. This is just for debug and not planned to be a game feature. Maybe in level editor?
            Vector2 position = video.convertPixelToGame({event.button.x, event.button.y}, camera);
            position.x = std::round(position.x);
            position.y = std::round(position.y);
            auto things = findObjectsAtCoords(position);
            if (things.size() == 0) {
                if (auto obj = createGameObject().lock()) {
                    obj->transform.position = position;
                    obj->addTag("floor");
                }
            } else if (auto thing = things[0].lock()) {
                if (thing->hasTag("floor"))
                    destroyGameObject(thing);
            }
            break;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            auto it = keyActions.find(event.key.scancode);
            if (it != keyActions.end())
                it->second(event);
            break;
    }
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->handleEvent(event);
    }
}

void PlatformerScene::update(float deltaTime) {
    deltaTime *= timeSpeed;
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->update(deltaTime);
    }

    if (auto p = player.lock())
        camera.position += (p->transform.position - camera.position) * 5.0f * std::min(deltaTime, 0.2f);
}

Video::RenderInfo PlatformerScene::render() {
    Video::RenderInfo info;
    for (int i = 0; i < objects.size(); i++) {
        info.instances.push_back({Video::QUAD, objects[i]->transform, objects[i]->color});
    }
    info.camera = camera;
    return info;
}

void PlatformerScene::completeLevel() {
    Uint64 completionTimeMS = SDL_GetTicks() - startMS;
    float completionTimeSec = static_cast<float>(completionTimeMS) / 1000.0f;

    SDL_Log("Time to complete: %.4f", completionTimeSec);

    sceneManager->queueSwitchToScene(std::make_unique<TitleScreen>());
}

std::optional<Collision> PlatformerScene::checkCollisions(const GameObject& obj) {
    if (obj.velocity.x == 0.0f && obj.velocity.y == 0.0f)
        return std::nullopt;
    std::vector<Collision> collisions;
    for (auto& other : objects) {
        if (other.get() == &obj)
            continue;

        float left = other->transform.position.x - other->transform.scale.x / 2 - obj.transform.scale.x / 2;
        float right = other->transform.position.x + other->transform.scale.x / 2 + obj.transform.scale.x / 2;
        float bottom = other->transform.position.y - other->transform.scale.y / 2 - obj.transform.scale.y / 2;
        float top = other->transform.position.y + other->transform.scale.y / 2 + obj.transform.scale.y / 2;

        if (obj.velocity.x == 0.0f) {
            if (obj.transform.position.x <= left || obj.transform.position.x >= right) {
                continue;
            }
        } else if (obj.velocity.x > 0.0f) {
            if (obj.transform.position.x >= right)
                continue;
        } else {
            if (obj.transform.position.x <= left)
                continue;
        }
        if (obj.velocity.y == 0.0f) {
            if (obj.transform.position.y <= bottom || obj.transform.position.y >= top)
                continue;
        } else if (obj.velocity.y > 0.0f) {
            if (obj.transform.position.y >= top)
                continue;
        } else {
            if (obj.transform.position.y <= bottom)
                continue;
        }

        float xEntry = ((obj.velocity.x >= 0 ? left : right) - obj.transform.position.x) / obj.velocity.x;
        float xExit =  ((obj.velocity.x >= 0 ? right : left) - obj.transform.position.x) / obj.velocity.x;
        float yEntry = ((obj.velocity.y >= 0 ? bottom : top) - obj.transform.position.y) / obj.velocity.y;
        float yExit =  ((obj.velocity.y >= 0 ? top : bottom) - obj.transform.position.y) / obj.velocity.y;


        if (xEntry > yExit || yEntry > xExit)
            continue;

        if (xEntry < 0.0f && yEntry < 0.0f)
            continue;

        Collision collision = {};
        collision.other = other;
        if (xEntry > yEntry) {
            collision.time = xEntry;
            collision.normal = obj.velocity.x > 0 ? Vector2(-1, 0) : Vector2(1, 0);
        } else {
            collision.time = yEntry;
            collision.normal = obj.velocity.y > 0 ? Vector2(0, -1) : Vector2(0, 1);
        }
        collisions.push_back(collision);
    }


    if (collisions.size() == 0)
        return std::nullopt;

    // Find the earliest collision
    Collision& min = collisions[0];
    std::vector<Collision> minTies;
    for (Collision& c : collisions) {
        if (c.time < min.time) {
            min = c;
            minTies.clear();
        }
        if (c.time == min.time) {
            minTies.push_back(c);
        }
    }


    // If there's no tie, return the earliest collision
    if (minTies.size() <= 1)
        return min;

    // Break ties by choosing the collision with the object closest to the main object
    Collision& closest = minTies[0];
    for (Collision& c : minTies) {
        if (auto other = c.other.lock()) {
            if (auto closestOther = closest.other.lock()) {
                if (other->transform.position.distanceSquared(obj.transform.position) < closestOther->transform.position.distanceSquared(obj.transform.position)) {
                    closest = c;
                }
            }
        }
    }

    return closest;
}

std::vector<std::weak_ptr<GameObject>> PlatformerScene::findObjectsAtCoords(Vector2 pos) {
    std::vector<std::weak_ptr<GameObject>> output;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->transform.position == pos) {
            output.push_back(objects[i]);
        }
    }
    return output;
}

std::weak_ptr<GameObject> PlatformerScene::createGameObject() {
    auto obj = std::make_shared<GameObject>(this);
    objects.push_back(obj);
    return obj;
}

void PlatformerScene::destroyGameObject(const std::weak_ptr<GameObject> obj) {
    if (auto objP = obj.lock()) {
        auto it = std::find(objects.begin(), objects.end(), objP);
        if (it != objects.end()) {
            objects.erase(it);
        }
    }
}
