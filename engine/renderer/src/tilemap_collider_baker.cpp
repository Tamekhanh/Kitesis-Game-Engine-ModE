#include "engine/renderer/tilemap_collider_baker.h"
#include "engine/renderer/tilemap_component.h"
#include "engine/core/tilemap_collider_component.h"
#include "engine/core/component_registry.h"
#include <vector>

namespace engine::renderer
{

    void BakeTilemapCollider(TilemapComponent &tilemap, engine::core::TilemapColliderComponent &outCollider)
    {
        outCollider.rects.clear();

        int w = tilemap.Width();
        int h = tilemap.Height();
        float tileSize = tilemap.TileSize();

        std::vector<char> visited(w * h, 0);
        auto solid = [&](int x, int y)
        { return tilemap.GetTile(x, y) >= 0; };
        auto vis = [&](int x, int y) -> char &
        { return visited[y * w + x]; };

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                if (!solid(x, y) || vis(x, y))
                    continue;

                int runWidth = 1;
                while (x + runWidth < w && solid(x + runWidth, y) && !vis(x + runWidth, y))
                {
                    runWidth++;
                }

                int runHeight = 1;
                bool canExtend = true;
                while (canExtend && y + runHeight < h)
                {
                    for (int dx = 0; dx < runWidth; ++dx)
                    {
                        if (!solid(x + dx, y + runHeight) || vis(x + dx, y + runHeight))
                        {
                            canExtend = false;
                            break;
                        }
                    }
                    if (canExtend)
                        runHeight++;
                }

                for (int dy = 0; dy < runHeight; ++dy)
                {
                    for (int dx = 0; dx < runWidth; ++dx)
                    {
                        vis(x + dx, y + dy) = 1;
                    }
                }

                engine::core::ColliderRect rect;
                rect.offsetX = x * tileSize;
                rect.offsetY = y * tileSize;
                rect.width = runWidth * tileSize;
                rect.height = runHeight * tileSize;
                outCollider.rects.push_back(rect);
            }
        }
    }

    static engine::core::ComponentAutoRegister s_tilemapColliderRegister(
        "TilemapColliderComponent",
        [](engine::core::GameObject &obj)
        {
            auto *tilemapCollider = obj.AddComponent<engine::core::TilemapColliderComponent>();

            // Neu GameObject nay da co san TilemapComponent, bake ngay lap tuc
            auto *tilemap = obj.GetComponent<TilemapComponent>();
            if (tilemap)
            {
                BakeTilemapCollider(*tilemap, *tilemapCollider);
            }
        });

} // namespace engine::renderer