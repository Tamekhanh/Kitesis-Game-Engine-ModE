#include "engine/core/collider_component.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"

namespace engine::core
{

    void ColliderComponent::GetWorldBounds(float &outMinX, float &outMinY, float &outMaxX, float &outMaxY) const
    {
        auto worldPos = Owner()->GetWorldPosition();
        float cx = worldPos.x + offset.x;
        float cy = worldPos.y + offset.y;

        outMinX = cx - size.x * 0.5f;
        outMinY = cy - size.y * 0.5f;
        outMaxX = cx + size.x * 0.5f;
        outMaxY = cy + size.y * 0.5f;
    }

    bool ColliderComponent::Overlaps(const ColliderComponent &a, const ColliderComponent &b)
    {
        float aMinX, aMinY, aMaxX, aMaxY;
        float bMinX, bMinY, bMaxX, bMaxY;
        a.GetWorldBounds(aMinX, aMinY, aMaxX, aMaxY);
        b.GetWorldBounds(bMinX, bMinY, bMaxX, bMaxY);

        return aMinX < bMaxX && aMaxX > bMinX &&
               aMinY < bMaxY && aMaxY > bMinY;
    }

    static ComponentAutoRegister s_colliderRegister(
        "ColliderComponent",
        [](GameObject &obj)
        {
            obj.AddComponent<ColliderComponent>();
        });

} // namespace engine::core