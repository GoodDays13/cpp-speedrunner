#pragma once

#include <SDL3/SDL_iostream.h>
#include <string>
#include <optional>
#include "LevelData.h"

class LevelLoader {
public:
    static std::optional<LevelData> loadLevel(const std::string& levelName);

private:
    static std::optional<LevelData> loadJsonLevel(SDL_IOStream* stream);
    static std::optional<LevelData> loadBinaryLevel(SDL_IOStream* stream);
};
