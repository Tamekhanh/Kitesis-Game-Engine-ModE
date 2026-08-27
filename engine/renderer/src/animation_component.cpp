#include "engine/renderer/animation_component.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"
#include <algorithm>

namespace engine::renderer {

void AnimationComponent::OnStart() {
    m_sprite = Owner()->GetComponent<SpriteComponent>();
    if (m_sprite && !m_frames.empty()) {
        ApplyFrame(0);
    }
}

Texture2D* AnimationComponent::GetOrLoadTexture(const std::string& path) {
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) return it->second.get();

    auto tex = std::make_unique<Texture2D>(path);
    Texture2D* raw = tex.get();
    m_textureCache[path] = std::move(tex);
    return raw;
}

unsigned int AnimationComponent::GetOrLoadThumbnailId(const std::string& texturePath) {
    auto* tex = GetOrLoadTexture(texturePath);
    return tex ? tex->Id() : 0;
}

void AnimationComponent::ApplyFrame(int index) {
    if (!m_sprite || m_frames.empty()) return;
    if (index < 0 || index >= (int)m_frames.size()) return;

    auto& f = m_frames[index];

    if (f.texturePath != m_currentTexturePath) {
        auto* tex = GetOrLoadTexture(f.texturePath);
        if (tex) {
            m_sprite->BindTexture(*tex, f.texturePath);
            m_currentTexturePath = f.texturePath;
        }
    }
    m_sprite->SetFrame(f.frameIndex);
}

void AnimationComponent::AddFrame(const std::string& texturePath, int frameIndex) {
    m_frames.push_back({texturePath, frameIndex});
}

void AnimationComponent::RemoveFrame(int index) {
    if (index < 0 || index >= (int)m_frames.size()) return;
    m_frames.erase(m_frames.begin() + index);
    m_currentIndex = std::min(m_currentIndex, (int)m_frames.size() - 1);
}

void AnimationComponent::MoveFrame(int index, int direction) {
    int target = index + direction;
    if (index < 0 || index >= (int)m_frames.size()) return;
    if (target < 0 || target >= (int)m_frames.size()) return;
    std::swap(m_frames[index], m_frames[target]);
}

void AnimationComponent::ClearFrames() {
    m_frames.clear();
    m_currentIndex = 0;
    m_elapsed = 0.0f;
}

void AnimationComponent::Play() { m_playing = true; }
void AnimationComponent::Stop() { m_playing = false; }

void AnimationComponent::OnUpdate(float deltaTime) {
    if (!m_playing || m_frames.empty()) return;

    if (!m_sprite) {
        m_sprite = Owner()->GetComponent<SpriteComponent>();
        if (!m_sprite) return;
    }

    m_elapsed += deltaTime;

    if (m_elapsed >= m_frameDuration) {
        m_elapsed -= m_frameDuration;
        m_currentIndex++;

        if (m_currentIndex >= (int)m_frames.size()) {
            if (m_looping) {
                m_currentIndex = 0;
            } else {
                m_currentIndex = (int)m_frames.size() - 1;
                m_playing = false;
            }
        }
        ApplyFrame(m_currentIndex);
    }
}

static engine::core::ComponentAutoRegister s_animationRegister(
    "AnimationComponent",
    [](engine::core::GameObject& obj) {
        obj.AddComponent<AnimationComponent>();
    }
);

} // namespace engine::renderer