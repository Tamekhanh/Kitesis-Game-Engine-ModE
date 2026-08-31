#include "engine/core/physics_component.h"
#include "engine/core/component_registry.h"

namespace engine::core
{

    static ComponentAutoRegister s_physicsRegister(
        "PhysicsComponent",
        [](GameObject &obj)
        {
            obj.AddComponent<PhysicsComponent>();
        });

} // namespace engine::core