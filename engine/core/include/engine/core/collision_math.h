#pragma once
#include "engine/math/vector2.h"

namespace engine::core {

class BoxColliderComponent;
class CircleColliderComponent;

struct CollisionResult {
    bool colliding = false;
    engine::math::Vector2 normal{0.0f, 0.0f};
    float depth = 0.0f;
};

CollisionResult TestBoxBox(const BoxColliderComponent& a, const BoxColliderComponent& b);
CollisionResult TestCircleCircle(const CircleColliderComponent& a, const CircleColliderComponent& b);
CollisionResult TestBoxCircle(const BoxColliderComponent& box, const CircleColliderComponent& circle);

} // namespace engine::core