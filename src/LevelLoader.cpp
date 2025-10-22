#include "LevelLoader.h"
#include "JsonReader.h"
#include "LevelData.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <cstdint>
#include <optional>
#include <string>

std::optional<LevelData> LevelLoader::loadLevel(const std::string& levelName) {
    SDL_IOStream* stream = nullptr;
    std::optional<LevelData> result = std::nullopt;
    std::string levelPath = SDL_GetBasePath();
    levelPath += "assets/levels/" + levelName;

    if (levelName.ends_with(".json")) {
        stream = SDL_IOFromFile(levelPath.c_str(), "r");
        if (!stream) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        } else {
            result = loadJsonLevel(stream);
        }
    } else if (levelName.ends_with(".bin") || levelName.ends_with(".lvl")) {
        stream = SDL_IOFromFile(levelPath.c_str(), "rb");
        if (!stream) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        } else {
            result = loadBinaryLevel(stream);
        }
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown level format: %s", levelName.c_str());
    }

    if (stream) SDL_CloseIO(stream);

    return result;
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
        ObjectData obj = {
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
    SDL_ReadIO(stream, &level_data, sizeof(LevelData) - sizeof(std::vector<struct ObjectData>));
    ObjectData obj;
    while (SDL_ReadIO(stream, &obj, sizeof(struct ObjectData)) == sizeof(struct ObjectData)) {
        level_data.objects.push_back(obj);
    }
    return level_data;
}
