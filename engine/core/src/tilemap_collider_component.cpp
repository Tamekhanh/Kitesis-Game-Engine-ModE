#include "engine/core/tilemap_collider_component.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"

namespace engine::core
{

    void TilemapColliderComponent::GetWorldBounds(int index, float &outMinX, float &outMinY,
                                                  float &outMaxX, float &outMaxY) const
    {
        if (index < 0 || index >= (int)rects.size())
            return;
        auto worldPos = Owner()->GetWorldPosition();
        auto &r = rects[index];
        outMinX = worldPos.x + r.offsetX;
        outMinY = worldPos.y + r.offsetY;
        outMaxX = outMinX + r.width;
        outMaxY = outMinY + r.height;
    }

} // namespace engine::core