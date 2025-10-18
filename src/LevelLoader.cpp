#include "LevelLoader.h"
#include "JsonReader.h"
#include "LevelData.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <cstdint>
#include <optional>
#include <string>

std::optional<LevelData> LevelLoader::loadLevel(const std::string& levelName) {
    std::string levelPath = SDL_GetBasePath();
    levelPath += "assets/levels/" + levelName;

    Format format = detectFormat(levelName);
    if (format == Format::UNKNOWN) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown level format: %s", levelName.c_str());
        return std::nullopt;
    }

    SDL_IOStream* stream = SDL_IOFromFile(levelPath.c_str(), (format == Format::JSON) ? "r" : "rb");
    if (!stream) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return std::nullopt;
    }

    std::optional<LevelData> result = std::nullopt;
    switch (format) {
        case Format::JSON:
            result = loadJsonLevel(stream);
            break;
        case Format::BINARY:
            result = loadBinaryLevel(stream);
            break;
        case Format::UNKNOWN:
            break;
    }
    SDL_CloseIO(stream);
    return result;
}

LevelLoader::Format LevelLoader::detectFormat(const std::string& filename) {
    if (filename.ends_with(".json"))
        return Format::JSON;
    else if (filename.ends_with(".bin") || filename.ends_with(".lvl"))
        return Format::BINARY;
    else
        return Format::UNKNOWN;

}

std::optional<LevelData> LevelLoader::loadJsonLevel(SDL_IOStream* stream) {
    auto jsonReader = JsonReader(stream);
    auto jsonValue = jsonReader.readJsonFile();

    LevelData levelData = {};

    levelData.player = {
        { // transform
            Vector2( // position
                jsonValue["player"]["position"]["x"].getDouble(),
                jsonValue["player"]["position"]["y"].getDouble()
            ),
            Vector2( // scale
                jsonValue["player"]["scale"]["x"].getDouble(),
                jsonValue["player"]["scale"]["y"].getDouble()
            )
        },
        Vector4( // color
            jsonValue["player"]["color"]["r"].getDouble(),
            jsonValue["player"]["color"]["g"].getDouble(),
            jsonValue["player"]["color"]["b"].getDouble(),
            jsonValue["player"]["color"]["a"].getDouble()
        )
    };

    JsonValue objects = jsonValue["objects"];
    if (!objects.isArray())
        return std::nullopt;

    std::unordered_map tagMap = std::unordered_map<std::string, int>();
    tagMap["floor"] = Tags::Floor;
    tagMap["end"] = Tags::End;
    tagMap["kill"] = Tags::Kill;
    tagMap["no_draw"] = Tags::NoDraw;

    for (int i = 0; i < objects.size(); i++) {
        JsonValue object = jsonValue["objects"][i];
        Object obj = {
            { // transform
                Vector2( // position
                    object["position"]["x"].getDouble(),
                    object["position"]["y"].getDouble()
                ),
                Vector2( // scale
                    object["scale"]["x"].getDouble(),
                    object["scale"]["y"].getDouble()
                )
            },
            Vector4( // color
                object["color"]["r"].getDouble(),
                object["color"]["g"].getDouble(),
                object["color"]["b"].getDouble(),
                object["color"]["a"].getDouble()
            )
        };

        if (!object["tags"].isArray())
            continue;

        uint32_t tags = 0;

        for (int i = 0; i < object["tags"].size(); i++) {
            tags |= tagMap[object["tags"][i].getString()];
        }
        obj.tags = tags;

        levelData.objects.push_back(obj);
    }

    return levelData;
}

std::optional<LevelData> LevelLoader::loadBinaryLevel(SDL_IOStream* stream) {
    LevelData level_data;
    SDL_ReadIO(stream, &level_data, sizeof(LevelData) - sizeof(std::vector<struct Object>));
    Object obj;
    while (SDL_ReadIO(stream, &obj, sizeof(struct Object)) == sizeof(struct Object)) {
        level_data.objects.push_back(obj);
    }
    return level_data;
}
