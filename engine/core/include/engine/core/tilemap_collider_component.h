#pragma once
#include "engine/core/component.h"
#include <vector>

namespace engine::core
{

    struct ColliderRect
    {
        float offsetX, offsetY;
        float width, height;
    };

    class TilemapColliderComponent : public Component
    {
    public:
        std::vector<ColliderRect> rects;
        bool isTrigger = false;

        void GetWorldBounds(int index, float &outMinX, float &outMinY, float &outMaxX, float &outMaxY) const;
    };

} // namespace engine::core