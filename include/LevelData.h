#pragma once

#include "math.h"
#include <cstdint>
#include <vector>

enum Tags {
    Floor =     1 << 0,
    End =       1 << 1,
    Kill =      1 << 2,
    NoDraw =   1 << 3
};

struct Object {
    Transform transform;
    Vector4 color;
    uint32_t tags;
};

struct LevelData {
    struct PlayerData {
        Transform transform;
        Vector4 color;
    } player;
    std::vector<Object> objects;
};
