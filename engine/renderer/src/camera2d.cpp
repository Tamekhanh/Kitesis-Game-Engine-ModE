#include "engine/renderer/camera2d.h"

namespace engine::renderer
{

    Camera2D::Camera2D(float width, float height)
        : m_width(width), m_height(height)
    {
        RecalculateProjection();
    }

    void Camera2D::Resize(float width, float height)
    {
        m_width = width;
        m_height = height;
        RecalculateProjection();
    }

    void Camera2D::RecalculateProjection()
    {
        // top=0, bottom=m_height => gốc toạ độ ở góc trên-trái, Y hướng xuống
        m_projection = engine::math::Ortho(0.0f, m_width, m_height, 0.0f, -1.0f, 1.0f);
    }

} // namespace engine::renderer
