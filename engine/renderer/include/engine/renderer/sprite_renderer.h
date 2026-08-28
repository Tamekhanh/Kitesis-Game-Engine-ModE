#pragma once
#include <memory>

namespace engine::renderer
{

    class Shader;
    class Camera2D;
    class Texture2D;

    class SpriteRenderer
    {
    public:
        SpriteRenderer();
        ~SpriteRenderer();

        void Begin(const Camera2D &camera);
        void DrawQuad(float x, float y, float width, float height,
                      float r, float g, float b);
        void DrawTexturedQuad(float x, float y, float width, float height,
                              const Texture2D &texture);

        // Ban moi: ve dung 1 vung UV cu the tren texture (dung cho sprite sheet)
        void DrawTexturedQuadUV(float x, float y, float width, float height,
                                const Texture2D &texture,
                                float uMin, float vMin, float uMax, float vMax);

    private:
        void UpdateQuadUVs(float uMin, float vMin, float uMax, float vMax);

        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        std::unique_ptr<Shader> m_shader;
    };

} // namespace engine::renderer