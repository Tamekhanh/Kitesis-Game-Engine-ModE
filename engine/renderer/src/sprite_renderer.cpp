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

uniform vec2 uAnchor;     // vi tri world cua Pivot (chinh la Transform.position)
uniform vec2 uScale;      // width, height
uniform vec2 uPivot;      // 0..1, diem neo trong pham vi sprite
uniform float uRotation;  // radian
uniform mat4 uProjection;

void main() {
    // Buoc 1+2: toa do cuc bo, da tru di do lech Pivot (goc xoay = Pivot)
    vec2 local = aPos * uScale + (vec2(0.5, 0.5) - uPivot) * uScale;

    // Buoc 3: xoay quanh Pivot (goc toa do cuc bo luc nay)
    float c = cos(uRotation);
    float s = sin(uRotation);
    vec2 rotated = vec2(local.x * c - local.y * s, local.x * s + local.y * c);

    // Buoc 4: dich chuyen theo vi tri world cua Pivot
    vec2 pos = uAnchor + rotated;

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
    float uvs[] = {
        uMin, vMax, uMax, vMax, uMax, vMin,
        uMin, vMax, uMax, vMin, uMin, vMin,
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    for (int i = 0; i < 6; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, (i * 4 + 2) * sizeof(float), 2 * sizeof(float), &uvs[i * 2]);
    }
}

void SpriteRenderer::DrawQuad(float x, float y, float width, float height,
                               float r, float g, float b,
                               float pivotX, float pivotY, float rotationRadians) {
    UpdateQuadUVs(0.0f, 0.0f, 1.0f, 1.0f);

    unsigned int id = m_shader->Id();
    glUniform2f(glGetUniformLocation(id, "uAnchor"), x, y);
    glUniform2f(glGetUniformLocation(id, "uScale"), width, height);
    glUniform2f(glGetUniformLocation(id, "uPivot"), pivotX, pivotY);
    glUniform1f(glGetUniformLocation(id, "uRotation"), rotationRadians);
    glUniform3f(glGetUniformLocation(id, "uColor"), r, g, b);
    glUniform1i(glGetUniformLocation(id, "uUseTexture"), 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::DrawTexturedQuad(float x, float y, float width, float height,
                                       const Texture2D& texture,
                                       float pivotX, float pivotY, float rotationRadians) {
    DrawTexturedQuadUV(x, y, width, height, texture, 0.0f, 0.0f, 1.0f, 1.0f, pivotX, pivotY, rotationRadians);
}

void SpriteRenderer::DrawTexturedQuadUV(float x, float y, float width, float height,
                                        const Texture2D& texture,
                                        float uMin, float vMin, float uMax, float vMax,
                                        float pivotX, float pivotY, float rotationRadians) {
    UpdateQuadUVs(uMin, vMin, uMax, vMax);

    unsigned int id = m_shader->Id();
    texture.Bind(0);
    glUniform2f(glGetUniformLocation(id, "uAnchor"), x, y);
    glUniform2f(glGetUniformLocation(id, "uScale"), width, height);
    glUniform2f(glGetUniformLocation(id, "uPivot"), pivotX, pivotY);
    glUniform1f(glGetUniformLocation(id, "uRotation"), rotationRadians);
    glUniform1i(glGetUniformLocation(id, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(id, "uUseTexture"), 1);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace engine::renderer