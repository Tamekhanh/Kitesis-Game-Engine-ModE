#pragma once
#include "engine/core/component.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace engine::renderer
{

    class SpriteComponent;
    class Texture2D;

    struct AnimationFrame
    {
        std::string texturePath;
        int frameIndex = 0;
    };

    class AnimationComponent : public engine::core::Component
    {
    public:
        void OnStart() override;
        void OnUpdate(float deltaTime) override;

        void AddFrame(const std::string &texturePath, int frameIndex);
        void RemoveFrame(int index);
        void MoveFrame(int index, int direction); // -1: len tren, +1: xuong duoi
        void ClearFrames();

        void SetFrameDuration(float seconds) { m_frameDuration = seconds; }
        void SetLooping(bool loop) { m_looping = loop; }
        void Play();
        void Stop();

        bool IsPlaying() const { return m_playing; }
        int CurrentIndex() const { return m_currentIndex; }
        const std::vector<AnimationFrame> &Frames() const { return m_frames; }
        float FrameDuration() const { return m_frameDuration; }
        bool Looping() const { return m_looping; }

        // Dung cho panel Animation: lay texture id de hien thumbnail dung anh that
        unsigned int GetOrLoadThumbnailId(const std::string &texturePath);

    private:
        void ApplyFrame(int index);
        Texture2D *GetOrLoadTexture(const std::string &path);

        SpriteComponent *m_sprite = nullptr;
        std::vector<AnimationFrame> m_frames;
        std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_textureCache;
        std::string m_currentTexturePath;

        float m_frameDuration = 0.1f;
        bool m_looping = true;
        bool m_playing = true;
        int m_currentIndex = 0;
        float m_elapsed = 0.0f;
    };

} // namespace engine::renderer