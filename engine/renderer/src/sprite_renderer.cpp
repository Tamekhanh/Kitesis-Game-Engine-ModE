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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

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

void SpriteRenderer::UpdateQuadUVs(float uMin, float vMin, float uMax, float vMax) {
    // Cap nhat lai 4 gia tri UV (2 so cuoi cua moi vertex) trong VBO hien co
    float uvs[] = {
        uMin, vMax,
        uMax, vMax,
        uMax, vMin,
        uMin, vMax,
        uMax, vMin,
        uMin, vMin,
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    for (int i = 0; i < 6; ++i) {
        // Moi vertex chiem 4 float (x,y,u,v), UV nam o offset +2
        glBufferSubData(GL_ARRAY_BUFFER,
                         (i * 4 + 2) * sizeof(float),
                         2 * sizeof(float),
                         &uvs[i * 2]);
    }
}

void SpriteRenderer::DrawQuad(float x, float y, float width, float height,
                               float r, float g, float b) {
    UpdateQuadUVs(0.0f, 0.0f, 1.0f, 1.0f);

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
    DrawTexturedQuadUV(x, y, width, height, texture, 0.0f, 0.0f, 1.0f, 1.0f);
}

void SpriteRenderer::DrawTexturedQuadUV(float x, float y, float width, float height,
                                        const Texture2D& texture,
                                        float uMin, float vMin, float uMax, float vMax) {
    UpdateQuadUVs(uMin, vMin, uMax, vMax);

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