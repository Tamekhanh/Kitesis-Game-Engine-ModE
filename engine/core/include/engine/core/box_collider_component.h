#pragma once
#include "engine/core/collider_component.h"

namespace engine::core {

class BoxColliderComponent : public ColliderComponent {
public:
    engine::math::Vector2 size{32.0f, 32.0f};

    ColliderShape Shape() const override { return ColliderShape::Box; }

    void GetCorners(engine::math::Vector2 outCorners[4]) const;
    void GetAABB(float& outMinX, float& outMinY, float& outMaxX, float& outMaxY) const;
};

} // namespace engine::core