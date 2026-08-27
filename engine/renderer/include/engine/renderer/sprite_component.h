#pragma once
#include "engine/core/component.h"
#include "engine/renderer/sprite_sheet_meta.h"
#include <string>

namespace engine::renderer {

class SpriteRenderer;
class Texture2D;

class SpriteComponent : public engine::core::Component {
public:
    SpriteComponent(SpriteRenderer& renderer, float width, float height);

    unsigned int PreviewTextureId() const;

    void BindTexture(Texture2D& texture, const std::string& texturePath);
    void OnRender() override;

    const std::string& TexturePath() const { return m_texturePath; }
    float Width() const { return m_width; }
    float Height() const { return m_height; }

    // --- Sprite Sheet ---
    void SetGrid(int columns, int rows);
    void SetFrame(int frameIndex);
    int Columns() const { return m_columns; }
    int Rows() const { return m_rows; }
    int FrameIndex() const { return m_frameIndex; }

    static void SetDefaultRenderer(SpriteRenderer* renderer) { s_defaultRenderer = renderer; }
    static SpriteRenderer* DefaultRenderer() { return s_defaultRenderer; }

private:
    SpriteRenderer* m_renderer;
    Texture2D* m_texture = nullptr;
    float m_width;
    float m_height;
    std::string m_texturePath;

    int m_columns = 1;
    int m_rows = 1;
    int m_frameIndex = 0;

    static SpriteRenderer* s_defaultRenderer;
};

} // namespace engine::renderer