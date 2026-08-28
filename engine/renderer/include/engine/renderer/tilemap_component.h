#pragma once
#include "engine/core/component.h"
#include <vector>
#include <string>

namespace engine::renderer
{

    class SpriteRenderer;
    class Texture2D;

    class TilemapComponent : public engine::core::Component
    {
    public:
        void OnRender() override;
        void OnStart() override;

        unsigned int PreviewTextureId() const;

        void Resize(int width, int height);
        void BindTileset(Texture2D &texture, const std::string &texturePath);

        void SetTile(int x, int y, int tileIndex);
        int GetTile(int x, int y) const;

        int Width() const { return m_width; }
        int Height() const { return m_height; }
        float TileSize() const { return m_tileSize; }
        void SetTileSize(float size) { m_tileSize = size; }

        const std::string &TexturePath() const { return m_texturePath; }
        int TilesetColumns() const { return m_tilesetColumns; }
        int TilesetRows() const { return m_tilesetRows; }

        static void SetDefaultRenderer(SpriteRenderer *renderer) { s_defaultRenderer = renderer; }
        static SpriteRenderer *DefaultRenderer() { return s_defaultRenderer; }

    private:
        int Index(int x, int y) const { return y * m_width + x; }
        bool InBounds(int x, int y) const { return x >= 0 && y >= 0 && x < m_width && y < m_height; }

        SpriteRenderer *m_renderer = nullptr;
        Texture2D *m_texture = nullptr;
        std::string m_texturePath;
        int m_tilesetColumns = 1;
        int m_tilesetRows = 1;

        int m_width = 10;
        int m_height = 10;
        float m_tileSize = 32.0f;
        std::vector<int> m_tiles;

        static SpriteRenderer *s_defaultRenderer;
    };

} // namespace engine::renderer