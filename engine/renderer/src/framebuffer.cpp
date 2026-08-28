#include "engine/renderer/framebuffer.h"
#include <glad/gl.h>
#include <cstdio>

namespace engine::renderer
{

    Framebuffer::Framebuffer(int width, int height)
        : m_width(width), m_height(height)
    {
        Invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_colorTexture);
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
    }

    void Framebuffer::Invalidate()
    {
        if (m_fbo != 0)
        {
            glDeleteFramebuffers(1, &m_fbo);
            glDeleteTextures(1, &m_colorTexture);
            glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        }

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // --- Color attachment: texture chứa màu, sẽ hiện trong ImGui::Image ---
        glGenTextures(1, &m_colorTexture);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_colorTexture, 0);

        // --- Depth attachment: chỉ cần renderbuffer, không cần đọc lại làm texture ---
        glGenRenderbuffers(1, &m_depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, m_depthRenderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::printf("[ERROR] Framebuffer khong hoan chinh!\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;
        if (width == m_width && height == m_height)
            return;

        m_width = width;
        m_height = height;
        Invalidate();
    }

} // namespace engine::renderer