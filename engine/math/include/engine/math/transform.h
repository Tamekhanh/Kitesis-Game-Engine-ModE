#pragma once
#include "engine/math/vector2.h"

namespace engine::math
{

    struct Transform
    {
        Vector2 position{0.0f, 0.0f};
        float rotation = 0.0f; // độ (degree), không phải radian
        Vector2 scale{1.0f, 1.0f};
    };

} // namespace engine::math