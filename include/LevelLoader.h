#pragma once

#include <SDL3/SDL_iostream.h>
#include <string>
#include <optional>
#include "LevelData.h"

class LevelLoader {
public:
    enum class Format { JSON, BINARY, UNKNOWN };

    static std::optional<LevelData> loadLevel(const std::string& levelName);

private:
    static Format detectFormat(const std::string& filename);
    static std::optional<LevelData> loadJsonLevel(SDL_IOStream* stream);
    static std::optional<LevelData> loadBinaryLevel(SDL_IOStream* stream);
};
