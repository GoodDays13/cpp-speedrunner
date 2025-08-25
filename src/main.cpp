#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game.h"

int main() {
    Game game;
    if (!game.initialize())
        return 1;

    game.run();

    return 0;
}
