#pragma once
#include "engine/math/mat4.h"

namespace engine::renderer {

class Camera2D {
public:
    Camera2D(float width, float height);

    void Resize(float width, float height);
    const engine::math::Mat4& GetProjection() const { return m_projection; }

private:
    void RecalculateProjection();

    float m_width;
    float m_height;
    engine::math::Mat4 m_projection;
};

} // namespace engine::renderer
