#include "TitleScreen.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

void TitleScreen::initialize(ISceneManager* sceneManager) {
    this->sceneManager = sceneManager;

    // Initialize title and menu items
    title = GameObject();
    title.transform.position = {0, 3};
    title.transform.scale = {10, 2};

    GameObject startGame = GameObject();
    startGame.transform.position = {0, 1};
    startGame.transform.scale = {6, 1};
    menuItems.push_back({[this]() {
        this->sceneManager->startLevel("output.lvl");
    }, startGame});

    GameObject quitGame = GameObject();
    quitGame.transform.position = {0, -1};
    quitGame.transform.scale = {6, 1};
    menuItems.push_back({[this]() {
        this->sceneManager->queuePopScene(this);
    }, quitGame});
}

void TitleScreen::handleEvent(SDL_Event event, const Video& video) {
    if (event.type == SDL_EVENT_KEY_UP) {
        switch (event.key.scancode) {
            case SDL_SCANCODE_SPACE:
            case SDL_SCANCODE_KP_ENTER:
            case SDL_SCANCODE_RETURN: {
                if (selectedItem < menuItems.size()) {
                    menuItems[selectedItem].first();
                }
                break;
            } case SDL_SCANCODE_ESCAPE:
                sceneManager->queuePopScene(this);
                break;
            default:
                break;
        }
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.scancode) {
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                if (selectedItem == menuItems.size()) selectedItem = 0; // If nothing is selected, select the first item
                else if (selectedItem > 0) selectedItem--;
                else selectedItem = menuItems.size() - 1; // Wrap around to the last item
                break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                if (selectedItem == menuItems.size()) selectedItem = 0; // If nothing is selected, select the first item
                else if (selectedItem < menuItems.size() - 1) selectedItem++;
                else selectedItem = 0; // Wrap around to the first item
                break;
            default:
                break;
        }
    } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
        Vector2 mousePos = video.convertPixelToGame({event.motion.x, event.motion.y}, {{0, 0}, {16, 9}});
        selectedItem = menuItems.size(); // Deselect if not hovering over any item
        for (size_t i = 0; i < menuItems.size(); i++) {
            GameObject& item = menuItems[i].second;
            if (mousePos.x >= item.transform.position.x - item.transform.scale.x / 2 &&
                mousePos.x <= item.transform.position.x + item.transform.scale.x / 2 &&
                mousePos.y >= item.transform.position.y - item.transform.scale.y / 2 &&
                mousePos.y <= item.transform.position.y + item.transform.scale.y / 2) {
                selectedItem = i;
            }
        }
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            Vector2 mousePos = video.convertPixelToGame({event.button.x, event.button.y}, {{0, 0}, {16, 9}});
            if (selectedItem < menuItems.size())
                menuItems[selectedItem].first();
        }
    }
}

void TitleScreen::update(float deltaTime) {
}

Video::RenderInfo TitleScreen::render() {
    Video::RenderInfo info;

    // Add title
    info.instances.push_back({Video::QUAD, {title.transform, {0, 1, 1, 1}}});

    for (size_t i = 0; i < menuItems.size(); i++) {
        Vector4 color = (i == selectedItem) ? Vector4{1, 1, 0, 1} : Vector4{1, 1, 1, 1}; // Highlight selected item in yellow
        Video::InstanceInfo menuItem = {Video::QUAD, {menuItems[i].second.transform, color}};
        info.instances.push_back(menuItem);
    }

    info.camera = {{0, 0}, {16, 9}};
    return info;
}
