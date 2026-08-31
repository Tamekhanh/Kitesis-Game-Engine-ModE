#pragma once
#include "engine/core/component.h"
#include "engine/math/vector2.h"

namespace engine::core
{
    enum class ColliderShape
    {
        Box,
        Circle
    };
    class ColliderComponent : public Component
    {
    public:
        engine::math::Vector2 offset{0.0f, 0.0f};
        bool isTrigger = false;
        virtual ColliderShape Shape() const = 0;
        engine::math::Vector2 WorldCenter() const;
    };
} // namespace engine::core