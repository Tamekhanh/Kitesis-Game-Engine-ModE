#include "engine/renderer/texture2d.h"
#include <glad/gl.h>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine::renderer {

Texture2D::Texture2D(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);

    int channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 0);

    if (!data) {
        std::printf("[ERROR] Failed to load texture: %s\n", path.c_str());
        return;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0,
                 format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    std::printf("[INFO] Loaded texture %s (%dx%d)\n", path.c_str(), m_width, m_height);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_id);
}

void Texture2D::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

} // namespace engine::renderer