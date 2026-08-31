#pragma once
#include "engine/core/collider_component.h"

namespace engine::core {

class CircleColliderComponent : public ColliderComponent {
public:
    float radius = 16.0f;

    ColliderShape Shape() const override { return ColliderShape::Circle; }
};

} // namespace engine::core