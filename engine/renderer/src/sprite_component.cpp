#include "engine/renderer/sprite_component.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"
#include <algorithm>

namespace engine::renderer
{

    SpriteRenderer *SpriteComponent::s_defaultRenderer = nullptr;

    SpriteComponent::SpriteComponent(SpriteRenderer &renderer, float width, float height)
        : m_renderer(&renderer), m_width(width), m_height(height) {}

    void SpriteComponent::BindTexture(Texture2D &texture, const std::string &texturePath)
    {
        m_texture = &texture;
        m_texturePath = texturePath;

        auto meta = SpriteSheetMeta::Load(texturePath);
        m_columns = meta.columns;
        m_rows = meta.rows;
        m_frameIndex = 0;
    }

    void SpriteComponent::SetGrid(int columns, int rows)
    {
        m_columns = std::max(1, columns);
        m_rows = std::max(1, rows);
        m_frameIndex = std::min(m_frameIndex, m_columns * m_rows - 1);
    }

    void SpriteComponent::SetFrame(int frameIndex)
    {
        int maxFrame = m_columns * m_rows - 1;
        m_frameIndex = std::clamp(frameIndex, 0, maxFrame);
    }

    void SpriteComponent::OnRender()
    {
        if (!m_texture)
            return;
        auto worldPos = Owner()->GetWorldPosition();

        if (m_columns <= 1 && m_rows <= 1)
        {
            m_renderer->DrawTexturedQuad(worldPos.x, worldPos.y, m_width, m_height, *m_texture);
            return;
        }

        int col = m_frameIndex % m_columns;
        int row = m_frameIndex / m_columns;

        float uMin = (float)col / (float)m_columns;
        float uMax = (float)(col + 1) / (float)m_columns;
        float vMin = (float)row / (float)m_rows;
        float vMax = (float)(row + 1) / (float)m_rows;

        m_renderer->DrawTexturedQuadUV(worldPos.x, worldPos.y, m_width, m_height, *m_texture,
                                       uMin, vMin, uMax, vMax);
    }
    unsigned int SpriteComponent::PreviewTextureId() const
    {
        return m_texture ? m_texture->Id() : 0;
    }

    static engine::core::ComponentAutoRegister s_spriteRegister(
        "SpriteComponent",
        [](engine::core::GameObject &obj)
        {
            auto *renderer = SpriteComponent::DefaultRenderer();
            if (!renderer)
                return;
            obj.AddComponent<SpriteComponent>(*renderer, 64.0f, 64.0f);
        });

} // namespace engine::renderer