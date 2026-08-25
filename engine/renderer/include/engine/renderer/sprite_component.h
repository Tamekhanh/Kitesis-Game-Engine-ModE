#pragma once
#include "engine/core/component.h"

namespace engine::renderer {

class SpriteRenderer;
class Texture2D;

class SpriteComponent : public engine::core::Component {
public:
    SpriteComponent(SpriteRenderer& renderer, Texture2D& texture,
                     float width, float height);

    void OnRender() override;

private:
    SpriteRenderer* m_renderer;
    Texture2D* m_texture;
    float m_width;
    float m_height;
};

} // namespace engine::renderer