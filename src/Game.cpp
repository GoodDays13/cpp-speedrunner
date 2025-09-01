#include "Game.h"
#include "TitleScreen.h"
#include "Video.h"
#include <memory>

bool Game::initialize() {
    video = std::make_unique<Video>();

    if (!video->initialize())
        return false;

    framerateLimit = video->getDisplayMode()->refresh_rate;

    return true;
}

void Game::run() {
    const Uint64 frameDelay = framerateLimit > 0 ? 1000000000ULL / framerateLimit : 0;

    pushScene(std::make_unique<TitleScreen>());

    lastFrameTime = SDL_GetTicksNS();
    isRunning = true;
    while (isRunning) {
        // Handle pending scene changes
        if (pendingSwitchScene) {
            switchToScene(std::move(pendingSwitchScene));
            pendingSwitchScene = nullptr;
            pendingPopScene = false;
            pendingPushScenes.clear();
        }
        if (pendingPopScene) {
            popScene();
            pendingPopScene = false;
        }
        for (auto& scene : pendingPushScenes) {
            pushScene(std::move(scene));
        }
        pendingPushScenes.clear();

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
            } else {
                sceneStack.back()->handleEvent(event, *video);
            }
        }

        sceneStack.back()->update(deltaTime);
        video->render(sceneStack.back()->render());
    }
}

void Game::pushScene(std::unique_ptr<IScene> scene) {
    scene->initialize(this);
    sceneStack.push_back(std::move(scene));
}

void Game::popScene() {
    if (sceneStack.empty())
        return;
    sceneStack.pop_back();
}

void Game::switchToScene(std::unique_ptr<IScene> scene) {
    while (!sceneStack.empty()) {
        popScene();
    }
    pushScene(std::move(scene));
}

void Game::queuePushScene(std::unique_ptr<IScene> scene) {
    pendingPushScenes.push_back(std::move(scene));
}

void Game::queuePopScene(const IScene* requestingScene) {
    if (!sceneStack.empty() && sceneStack.back().get() == requestingScene) {
        pendingPopScene = true;
    }
}

void Game::queueSwitchToScene(std::unique_ptr<IScene> scene) {
    pendingSwitchScene = std::move(scene);
}
