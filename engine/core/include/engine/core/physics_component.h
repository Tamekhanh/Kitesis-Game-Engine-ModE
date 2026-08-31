#pragma once
#include "engine/core/component.h"
#include "engine/math/vector2.h"

namespace engine::core
{

    class PhysicsComponent : public Component
    {
    public:
        engine::math::Vector2 velocity{0.0f, 0.0f};
        bool useGravity = true;
        float gravityScale = 1.0f;
        bool isKinematic = false; // true: khong bi day ra khi va cham solid, van di chuyen theo velocity

        float angularVelocity = 0.0f;
        float momentOfInertia = 1.0f; 
    };

} // namespace engine::core