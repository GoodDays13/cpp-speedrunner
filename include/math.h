#pragma once

#include <type_traits>

struct Vector2 {
    float x;
    float y;

    // Add two vectors
    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    // Subtract two vectors
    Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    // Scale by a scalar
    Vector2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    // Scale by a scalar
    Vector2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

    // Compound assignment
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    bool operator==(const Vector2& other) const {
        return this->x == other.x && this->y == other.y;
    }

    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    float distanceSquared(const Vector2& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return dx * dx + dy * dy;
    }
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};

struct Transform {
    Vector2 position;
    Vector2 scale;
};

static_assert(sizeof(Vector2) == sizeof(float) * 2, "Vector2 must be exactly 2 floats");
static_assert(std::is_trivial_v<Vector2>, "Vector2 must be trivial for graphics compatibility");
static_assert(sizeof(Vector3) == sizeof(float) * 3, "Vector3 must be exactly 3 floats");
static_assert(std::is_trivial_v<Vector3>, "Vector3 must be trivial for graphics compatibility");
static_assert(sizeof(Vector4) == sizeof(float) * 4, "Vector4 must be exactly 4 floats");
static_assert(std::is_trivial_v<Vector4>, "Vector4 must be trivial for graphics compatibility");
