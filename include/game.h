#include "gameobject.h"
#include "video.h"
#include <vector>

class Game {
public:
    Game();
    bool initialize();
    void run();
    void cleanup();
private:
    Video video;
    std::vector<GameObject*> objects;
    GameObject* player;
    unsigned long lastFrameTime;

    void processInput();
    void update(float deltaTime);
    void render();
    bool isRunning;
};
