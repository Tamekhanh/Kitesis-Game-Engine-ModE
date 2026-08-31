#include "engine/core/box_collider_component.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"
#include <cmath>
#include <algorithm>

namespace engine::core {

void BoxColliderComponent::GetCorners(engine::math::Vector2 outCorners[4]) const {
    auto center = WorldCenter();
    float rotRad = Owner()->transform.rotation * 0.01745329252f;
    float c = std::cos(rotRad);
    float s = std::sin(rotRad);

    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;

    engine::math::Vector2 local[4] = {
        {-hx, -hy}, {hx, -hy}, {hx, hy}, {-hx, hy}
    };

    for (int i = 0; i < 4; ++i) {
        float rx = local[i].x * c - local[i].y * s;
        float ry = local[i].x * s + local[i].y * c;
        outCorners[i] = { center.x + rx, center.y + ry };
    }
}

void BoxColliderComponent::GetAABB(float& outMinX, float& outMinY, float& outMaxX, float& outMaxY) const {
    engine::math::Vector2 corners[4];
    GetCorners(corners);

    outMinX = outMaxX = corners[0].x;
    outMinY = outMaxY = corners[0].y;
    for (int i = 1; i < 4; ++i) {
        outMinX = std::min(outMinX, corners[i].x);
        outMaxX = std::max(outMaxX, corners[i].x);
        outMinY = std::min(outMinY, corners[i].y);
        outMaxY = std::max(outMaxY, corners[i].y);
    }
}

static ComponentAutoRegister s_boxColliderRegister(
    "BoxColliderComponent",
    [](GameObject& obj) {
        obj.AddComponent<BoxColliderComponent>();
    }
);

} // namespace engine::core