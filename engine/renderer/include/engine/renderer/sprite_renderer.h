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
                      float r, float g, float b,
                      float pivotX = 0.0f, float pivotY = 0.0f,
                      float rotationRadians = 0.0f);

        void DrawTexturedQuad(float x, float y, float width, float height,
                              const Texture2D &texture,
                              float pivotX = 0.0f, float pivotY = 0.0f,
                              float rotationRadians = 0.0f);

        void DrawTexturedQuadUV(float x, float y, float width, float height,
                                const Texture2D &texture,
                                float uMin, float vMin, float uMax, float vMax,
                                float pivotX = 0.0f, float pivotY = 0.0f,
                                float rotationRadians = 0.0f);

    private:
        void UpdateQuadUVs(float uMin, float vMin, float uMax, float vMax);

        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        std::unique_ptr<Shader> m_shader;
    };

} // namespace engine::renderer