#pragma once

namespace engine::math
{

    struct Vector2
    {
        float x = 0.0f, y = 0.0f;

        Vector2 operator+(const Vector2 &o) const { return {x + o.x, y + o.y}; }
        Vector2 operator-(const Vector2 &o) const { return {x - o.x, y - o.y}; }
        Vector2 operator*(float s) const { return {x * s, y * s}; }
    };

}