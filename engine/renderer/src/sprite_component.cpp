#include "engine/renderer/sprite_component.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/game_object.h"

namespace engine::renderer {

SpriteComponent::SpriteComponent(SpriteRenderer& renderer, Texture2D& texture,
                                  float width, float height)
    : m_renderer(&renderer), m_texture(&texture),
      m_width(width), m_height(height) {}

void SpriteComponent::OnRender() {
    auto worldPos = Owner()->GetWorldPosition();
    m_renderer->DrawTexturedQuad(worldPos.x, worldPos.y, m_width, m_height, *m_texture);
}

} // namespace engine::renderer