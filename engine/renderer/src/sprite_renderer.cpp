#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/camera2d.h"
#include "engine/renderer/texture2d.h"
#include <glad/gl.h>

namespace engine::renderer {

static const char* kVertexSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
out vec2 vUV;
uniform vec2 uOffset;
uniform vec2 uScale;
uniform mat4 uProjection;
void main() {
    vec2 pos = aPos * uScale + uOffset;
    gl_Position = uProjection * vec4(pos, 0.0, 1.0);
    vUV = aUV;
}
)";

static const char* kFragmentSrc = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform vec3 uColor;
uniform sampler2D uTexture;
uniform int uUseTexture;
void main() {
    if (uUseTexture == 1) {
        FragColor = texture(uTexture, vUV);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}
)";

SpriteRenderer::SpriteRenderer() {
    float vertices[] = {
        -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_shader = std::make_unique<Shader>(kVertexSrc, kFragmentSrc);
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void SpriteRenderer::Begin(const Camera2D& camera) {
    m_shader->Bind();
    unsigned int id = m_shader->Id();
    int loc = glGetUniformLocation(id, "uProjection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, camera.GetProjection().m);
}

void SpriteRenderer::DrawQuad(float x, float y, float width, float height,
                               float r, float g, float b) {
    unsigned int id = m_shader->Id();
    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    glUniform2f(glGetUniformLocation(id, "uOffset"), centerX, centerY);
    glUniform2f(glGetUniformLocation(id, "uScale"), width, height);
    glUniform3f(glGetUniformLocation(id, "uColor"), r, g, b);
    glUniform1i(glGetUniformLocation(id, "uUseTexture"), 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::DrawTexturedQuad(float x, float y, float width, float height,
                                       const Texture2D& texture) {
    unsigned int id = m_shader->Id();
    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    texture.Bind(0);
    glUniform2f(glGetUniformLocation(id, "uOffset"), centerX, centerY);
    glUniform2f(glGetUniformLocation(id, "uScale"), width, height);
    glUniform1i(glGetUniformLocation(id, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(id, "uUseTexture"), 1);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace engine::renderer