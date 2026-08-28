#include "engine/renderer/tilemap_component.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/sprite_sheet_meta.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"
#include "engine/renderer/texture2d.h"

namespace engine::renderer
{

    SpriteRenderer *TilemapComponent::s_defaultRenderer = nullptr;

    void TilemapComponent::OnStart()
    {
        m_renderer = s_defaultRenderer;
    }

    unsigned int TilemapComponent::PreviewTextureId() const
    {
        return m_texture ? m_texture->Id() : 0;
    }

    void TilemapComponent::Resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        std::vector<int> newTiles(width * height, -1);

        int copyW = std::min(width, m_width);
        int copyH = std::min(height, m_height);
        for (int y = 0; y < copyH; ++y)
        {
            for (int x = 0; x < copyW; ++x)
            {
                newTiles[y * width + x] = m_tiles.empty() ? -1 : m_tiles[Index(x, y)];
            }
        }

        m_width = width;
        m_height = height;
        m_tiles = std::move(newTiles);
    }

    void TilemapComponent::BindTileset(Texture2D &texture, const std::string &texturePath)
    {
        m_texture = &texture;
        m_texturePath = texturePath;

        auto meta = SpriteSheetMeta::Load(texturePath);
        m_tilesetColumns = meta.columns;
        m_tilesetRows = meta.rows;
    }

    void TilemapComponent::SetTile(int x, int y, int tileIndex)
    {
        if (!InBounds(x, y))
            return;
        m_tiles[Index(x, y)] = tileIndex;
    }

    int TilemapComponent::GetTile(int x, int y) const
    {
        if (!InBounds(x, y))
            return -1;
        return m_tiles[Index(x, y)];
    }

    void TilemapComponent::OnRender()
    {
        if (!m_texture || !m_renderer)
            return;

        auto worldPos = Owner()->GetWorldPosition();

        for (int y = 0; y < m_height; ++y)
        {
            for (int x = 0; x < m_width; ++x)
            {
                int tileIndex = GetTile(x, y);
                if (tileIndex < 0)
                    continue;

                int col = tileIndex % m_tilesetColumns;
                int row = tileIndex / m_tilesetColumns;

                float uMin = (float)col / (float)m_tilesetColumns;
                float uMax = (float)(col + 1) / (float)m_tilesetColumns;
                float vMin = (float)row / (float)m_tilesetRows;
                float vMax = (float)(row + 1) / (float)m_tilesetRows;

                float px = worldPos.x + x * m_tileSize;
                float py = worldPos.y + y * m_tileSize;

                m_renderer->DrawTexturedQuadUV(px, py, m_tileSize, m_tileSize, *m_texture,
                                               uMin, vMin, uMax, vMax);
            }
        }
    }

    static engine::core::ComponentAutoRegister s_tilemapRegister(
        "TilemapComponent",
        [](engine::core::GameObject &obj)
        {
            auto *comp = obj.AddComponent<TilemapComponent>();
            comp->Resize(10, 10);
        });

} // namespace engine::renderer