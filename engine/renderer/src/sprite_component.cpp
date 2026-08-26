#include "engine/renderer/sprite_component.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/core/game_object.h"

namespace engine::renderer {

SpriteComponent::SpriteComponent(SpriteRenderer& renderer, float width, float height)
    : m_renderer(&renderer), m_width(width), m_height(height) {}

void SpriteComponent::BindTexture(Texture2D& texture, const std::string& texturePath) {
    m_texture = &texture;
    m_texturePath = texturePath;
}

void SpriteComponent::OnRender() {
    if (!m_texture) return; // chua gan texture, chua the ve
    auto worldPos = Owner()->GetWorldPosition();
    m_renderer->DrawTexturedQuad(worldPos.x, worldPos.y, m_width, m_height, *m_texture);
}

} // namespace engine::renderer