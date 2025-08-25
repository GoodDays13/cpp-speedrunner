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

    // Compound assignment
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
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
