#pragma once

namespace engine::renderer {

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Resize(int width, int height);

    unsigned int GetColorTextureId() const { return m_colorTexture; }
    int Width() const { return m_width; }
    int Height() const { return m_height; }

private:
    void Invalidate();

    unsigned int m_fbo = 0;
    unsigned int m_colorTexture = 0;
    unsigned int m_depthRenderbuffer = 0;
    int m_width;
    int m_height;
};

} // namespace engine::renderer