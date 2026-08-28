#pragma once
#include "engine/core/component.h"
#include "engine/math/vector2.h"

namespace engine::core
{

    class ColliderComponent : public Component
    {
    public:
        engine::math::Vector2 offset{0.0f, 0.0f};
        engine::math::Vector2 size{32.0f, 32.0f};
        bool isTrigger = false;

        // Vung AABB thuc te trong world space, da cong Transform + offset
        void GetWorldBounds(float &outMinX, float &outMinY, float &outMaxX, float &outMaxY) const;

        static bool Overlaps(const ColliderComponent &a, const ColliderComponent &b);
    };

} // namespace engine::core