#include "engine/core/circle_collider_component.h"
#include "engine/core/component_registry.h"

namespace engine::core {

static ComponentAutoRegister s_circleColliderRegister(
    "CircleColliderComponent",
    [](GameObject& obj) {
        obj.AddComponent<CircleColliderComponent>();
    }
);

} // namespace engine::core