#pragma once

namespace engine::math
{

    // Column-major 4x4 matrix, tương thích trực tiếp với glUniformMatrix4fv
    struct Mat4
    {
        float m[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
    };

    // Ma trận orthographic projection: left/right/bottom/top là biên toạ độ,
    // dùng để chuyển pixel -> NDC (-1..1)
    inline Mat4 Ortho(float left, float right, float bottom, float top,
                      float nearZ, float farZ)
    {
        Mat4 result{};
        for (float &v : result.m)
            v = 0.0f;

        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (farZ - nearZ);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(farZ + nearZ) / (farZ - nearZ);
        result.m[15] = 1.0f;

        return result;
    }

} // namespace engine::math
