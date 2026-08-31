#include "engine/core/collider_component.h"
#include "engine/core/game_object.h"

namespace engine::core {

engine::math::Vector2 ColliderComponent::WorldCenter() const {
    auto worldPos = Owner()->GetWorldPosition();
    return { worldPos.x + offset.x, worldPos.y + offset.y };
}

}