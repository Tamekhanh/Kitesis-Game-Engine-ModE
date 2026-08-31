#include "engine/core/collision_math.h"
#include "engine/core/box_collider_component.h"
#include "engine/core/circle_collider_component.h"
#include "engine/core/game_object.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace engine::core
{

    using engine::math::Vector2;

    static float Dot(const Vector2 &a, const Vector2 &b) { return a.x * b.x + a.y * b.y; }
    static float Length(const Vector2 &v) { return std::sqrt(v.x * v.x + v.y * v.y); }
    static Vector2 Normalize(const Vector2 &v)
    {
        float len = Length(v);
        if (len < 0.0001f)
            return {0.0f, 0.0f};
        return {v.x / len, v.y / len};
    }

    // Chieu 4 dinh cua 1 box len 1 truc, tra ve khoang [min, max]
    static void ProjectBox(const Vector2 corners[4], const Vector2 &axis, float &outMin, float &outMax)
    {
        outMin = outMax = Dot(corners[0], axis);
        for (int i = 1; i < 4; ++i)
        {
            float p = Dot(corners[i], axis);
            outMin = std::min(outMin, p);
            outMax = std::max(outMax, p);
        }
    }

    CollisionResult TestBoxBox(const BoxColliderComponent &a, const BoxColliderComponent &b)
    {
        CollisionResult result;

        Vector2 cornersA[4], cornersB[4];
        a.GetCorners(cornersA);
        b.GetCorners(cornersB);

        // 4 truc can kiem tra: 2 canh cua A, 2 canh cua B (SAT cho OBB 2D)
        Vector2 axes[4] = {
            Normalize({cornersA[1].x - cornersA[0].x, cornersA[1].y - cornersA[0].y}),
            Normalize({cornersA[3].x - cornersA[0].x, cornersA[3].y - cornersA[0].y}),
            Normalize({cornersB[1].x - cornersB[0].x, cornersB[1].y - cornersB[0].y}),
            Normalize({cornersB[3].x - cornersB[0].x, cornersB[3].y - cornersB[0].y}),
        };

        float minOverlap = std::numeric_limits<float>::max();
        Vector2 minAxis{0, 0};

        for (auto &axis : axes)
        {
            float minA, maxA, minB, maxB;
            ProjectBox(cornersA, axis, minA, maxA);
            ProjectBox(cornersB, axis, minB, maxB);

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (overlap <= 0.0f)
            {
                result.colliding = false;
                return result; // tim thay truc tach biet -> khong cham
            }
            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                minAxis = axis;
            }
        }

        // Dam bao normal huong dung tu B sang A
        Vector2 centerA = a.WorldCenter();
        Vector2 centerB = b.WorldCenter();
        Vector2 centerDiff = {centerA.x - centerB.x, centerA.y - centerB.y};
        if (Dot(centerDiff, minAxis) < 0.0f)
        {
            minAxis = {-minAxis.x, -minAxis.y};
        }

        result.colliding = true;
        result.normal = minAxis;
        result.depth = minOverlap;
        return result;
    }

    CollisionResult TestCircleCircle(const CircleColliderComponent &a, const CircleColliderComponent &b)
    {
        CollisionResult result;

        Vector2 centerA = a.WorldCenter();
        Vector2 centerB = b.WorldCenter();
        Vector2 diff = {centerA.x - centerB.x, centerA.y - centerB.y};
        float dist = Length(diff);
        float radiusSum = a.radius + b.radius;

        if (dist >= radiusSum || dist < 0.0001f)
        {
            result.colliding = false;
            return result;
        }

        result.colliding = true;
        result.normal = Normalize(diff);
        result.depth = radiusSum - dist;
        return result;
    }

    CollisionResult TestBoxCircle(const BoxColliderComponent &box, const CircleColliderComponent &circle)
    {
        CollisionResult result;

        Vector2 corners[4];
        box.GetCorners(corners);
        Vector2 boxCenter = box.WorldCenter();
        Vector2 circleCenter = circle.WorldCenter();

        float rotRad = box.Owner()->transform.rotation * 0.01745329252f;
        float c = std::cos(-rotRad), s = std::sin(-rotRad);
        Vector2 rel = {circleCenter.x - boxCenter.x, circleCenter.y - boxCenter.y};
        Vector2 localCircle = {rel.x * c - rel.y * s, rel.x * s + rel.y * c};

        float hx = box.size.x * 0.5f;
        float hy = box.size.y * 0.5f;

        Vector2 closest;
        closest.x = std::max(-hx, std::min(localCircle.x, hx));
        closest.y = std::max(-hy, std::min(localCircle.y, hy));

        // Doi chieu: tu diem gan nhat tren box HUONG VE TAM CIRCLE -> tra ve day "box"
        Vector2 diff = {closest.x - localCircle.x, closest.y - localCircle.y};
        float dist = Length(diff);

        if (dist >= circle.radius)
        {
            result.colliding = false;
            return result;
        }

        result.colliding = true;
        result.depth = circle.radius - dist;

        Vector2 localNormal = Normalize(diff);
        if (dist < 0.0001f)
            localNormal = {0.0f, 1.0f};

        float c2 = std::cos(rotRad), s2 = std::sin(rotRad);
        result.normal = {localNormal.x * c2 - localNormal.y * s2, localNormal.x * s2 + localNormal.y * c2};

        return result;
    }

} // namespace engine::core