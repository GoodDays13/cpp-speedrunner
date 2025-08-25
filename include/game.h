#include "video.h"

class Game {
public:
    Game();
    bool initialize();
    void run();
    void cleanup();
private:
    Video video;
    void processInput();
    void update();
    void render();
    bool isRunning;
};
