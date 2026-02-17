#pragma once

#include "GameObject.h"
#include "IGameWorld.h"
#include "IScene.h"
#include "LevelData.h"
#include "Player.h"
#include "Video.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class PlatformerScene : public IScene, public IGameWorld {
private:
    std::shared_ptr<LevelData> levelData;
    std::vector<std::shared_ptr<GameObject>> objects;
    std::weak_ptr<Player> player;
    Transform camera;
    float timeSpeed = 1.0f;
    std::unordered_map<SDL_Scancode, std::function<void(const SDL_Event&)>> keyActions;
    void loadLevel();
    void setupBinds();
    Uint64 startMS;
public:
    PlatformerScene() = delete;
    PlatformerScene(std::shared_ptr<LevelData> levelData) : levelData(levelData) {}
    ~PlatformerScene() = default;

    void initialize(ISceneManager* sceneManager) override;
    void handleEvent(SDL_Event event, const Video& video) override;
    void update(float deltaTime) override;
    Video::RenderInfo render() override;

    void completeLevel() override;

    std::vector<Collision> checkCollisions(const GameObject& obj) override;
    std::vector<std::weak_ptr<GameObject>> findObjectsAtCoords(Vector2 pos) override;
    std::weak_ptr<GameObject> createGameObject() override;
    void destroyGameObject(const std::weak_ptr<GameObject> obj) override;
};
