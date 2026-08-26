#pragma once
#include "engine/core/component.h"
#include <string>

namespace engine::renderer {

class SpriteRenderer;
class Texture2D;

class SpriteComponent : public engine::core::Component {
public:
    SpriteComponent(SpriteRenderer& renderer, float width, float height);

    void BindTexture(Texture2D& texture, const std::string& texturePath);

    void OnRender() override;

    const std::string& TexturePath() const { return m_texturePath; }
    float Width() const { return m_width; }
    float Height() const { return m_height; }

private:
    SpriteRenderer* m_renderer;
    Texture2D* m_texture = nullptr;
    float m_width;
    float m_height;
    std::string m_texturePath;
};

} // namespace engine::renderer