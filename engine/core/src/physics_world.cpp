#include "engine/core/physics_world.h"
#include "engine/core/physics_component.h"
#include "engine/core/box_collider_component.h"
#include "engine/core/circle_collider_component.h"
#include "engine/core/tilemap_collider_component.h"
#include "engine/core/collision_math.h"
#include <algorithm>
#include <cmath>

namespace engine::core
{

    static const float kGravity = 980.0f;
    static const float kRadToDeg = 57.29577951f;
    static const float kDegToRad = 0.01745329252f;

    std::set<std::pair<GameObject *, GameObject *>> PhysicsWorld::s_activeTriggers;

    void PhysicsWorld::Reset() { s_activeTriggers.clear(); }

    void PhysicsWorld::CollectBodies(GameObject &obj, std::vector<GameObject *> &out)
    {
        if (obj.GetComponent<PhysicsComponent>())
            out.push_back(&obj);
        for (auto &child : obj.Children())
            CollectBodies(*child, out);
    }

    void PhysicsWorld::CollectColliders(GameObject &obj, std::vector<ColliderComponent *> &out)
    {
        for (auto *c : obj.GetComponents<ColliderComponent>())
            out.push_back(c);
        for (auto &child : obj.Children())
            CollectColliders(*child, out);
    }

    static CollisionResult TestPair(ColliderComponent *a, ColliderComponent *b)
    {
        bool aIsBox = (a->Shape() == ColliderShape::Box);
        bool bIsBox = (b->Shape() == ColliderShape::Box);

        if (aIsBox && bIsBox)
        {
            return TestBoxBox(*static_cast<BoxColliderComponent *>(a), *static_cast<BoxColliderComponent *>(b));
        }
        if (!aIsBox && !bIsBox)
        {
            return TestCircleCircle(*static_cast<CircleColliderComponent *>(a), *static_cast<CircleColliderComponent *>(b));
        }
        if (aIsBox && !bIsBox)
        {
            return TestBoxCircle(*static_cast<BoxColliderComponent *>(a), *static_cast<CircleColliderComponent *>(b));
        }
        auto res = TestBoxCircle(*static_cast<BoxColliderComponent *>(b), *static_cast<CircleColliderComponent *>(a));
        res.normal = {-res.normal.x, -res.normal.y};
        return res;
    }

    // Ap dung ma sat lan don gian: dua angularVelocity ve gan gia tri "lan thuan tuy"
    // dua theo van toc tiep tuyen tai diem cham, chi ap dung neu owner la Circle
    static void ApplyRollingFriction(GameObject *owner, PhysicsComponent *phys, const engine::math::Vector2 &normal)
    {
        auto *circle = owner->GetComponent<CircleColliderComponent>();
        if (!circle || circle->radius <= 0.0001f)
            return;

        // Tiep tuyen: vuong goc voi normal (xoay normal 90 do)
        engine::math::Vector2 tangent = {-normal.y, normal.x};

        float vt = phys->velocity.x * tangent.x + phys->velocity.y * tangent.y;

        // Lan thuan tuy (khong truot): angularVelocity (radian/s) = -vt / radius
        // Dau am vi quy uoc xoay duong (CCW) ung voi lan sang trai theo tangent duong
        float targetAngularRad = -vt / circle->radius;
        float targetAngularDeg = targetAngularRad * kRadToDeg;

        float t = std::clamp(phys->rollingFriction, 0.0f, 1.0f);
        phys->angularVelocity = phys->angularVelocity + (targetAngularDeg - phys->angularVelocity) * t;
    }

    void PhysicsWorld::CollectTilemapBoxes(GameObject &obj, std::vector<StaticBox> &out)
    {
        if (auto *tc = obj.GetComponent<TilemapColliderComponent>())
        {
            for (int i = 0; i < (int)tc->rects.size(); ++i)
            {
                StaticBox box;
                box.owner = &obj;
                box.isTrigger = tc->isTrigger;
                tc->GetWorldBounds(i, box.minX, box.minY, box.maxX, box.maxY);
                out.push_back(box);
            }
        }
        for (auto &child : obj.Children())
            CollectTilemapBoxes(*child, out);
    }

    static CollisionResult TestStaticBoxVsCollider(const PhysicsWorld::StaticBox &box, ColliderComponent *other)
    {
        CollisionResult result;

        if (other->Shape() == ColliderShape::Circle)
        {
            auto *circle = static_cast<CircleColliderComponent *>(other);
            auto center = circle->WorldCenter();

            float closestX = std::max(box.minX, std::min(center.x, box.maxX));
            float closestY = std::max(box.minY, std::min(center.y, box.maxY));

            float dx = center.x - closestX;
            float dy = center.y - closestY;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist >= circle->radius)
                return result;

            result.colliding = true;
            result.depth = circle->radius - dist;
            if (dist < 0.0001f)
            {
                result.normal = {0.0f, -1.0f};
            }
            else
            {
                result.normal = {dx / dist, dy / dist};
            }
            return result;
        }

        // Box (khong xoay, dung AABB don gian vi tile khong xoay)
        auto *boxCollider = static_cast<BoxColliderComponent *>(other);
        auto center = boxCollider->WorldCenter();
        float hx = boxCollider->size.x * 0.5f;
        float hy = boxCollider->size.y * 0.5f;
        float oMinX = center.x - hx, oMaxX = center.x + hx;
        float oMinY = center.y - hy, oMaxY = center.y + hy;

        float overlapX = std::min(box.maxX, oMaxX) - std::max(box.minX, oMinX);
        float overlapY = std::min(box.maxY, oMaxY) - std::max(box.minY, oMinY);

        if (overlapX <= 0.0f || overlapY <= 0.0f)
            return result;

        result.colliding = true;
        float boxCenterX = (box.minX + box.maxX) * 0.5f;
        float boxCenterY = (box.minY + box.maxY) * 0.5f;

        // Normal huong tu tilemap-box ra phia other (dung quy uoc: day "other" ra khoi box)
        if (overlapX < overlapY)
        {
            result.normal = {(center.x < boxCenterX) ? -1.0f : 1.0f, 0.0f};
            result.depth = overlapX;
        }
        else
        {
            result.normal = {0.0f, (center.y < boxCenterY) ? -1.0f : 1.0f};
            result.depth = overlapY;
        }
        return result;
    }

    void PhysicsWorld::Step(GameObject &root, float deltaTime)
    {
        std::vector<GameObject *> bodies;
        CollectBodies(root, bodies);

        // 1. Trong luc, di chuyen tuyen tinh, tich hop xoay
        for (auto *body : bodies)
        {
            auto *phys = body->GetComponent<PhysicsComponent>();
            if (phys->useGravity && !phys->isKinematic)
            {
                phys->velocity.y += kGravity * phys->gravityScale * deltaTime;
            }
            body->transform.position.x += phys->velocity.x * deltaTime;
            body->transform.position.y += phys->velocity.y * deltaTime;

            body->transform.rotation += phys->angularVelocity * deltaTime;

            float damp = std::clamp(1.0f - phys->angularDamping * deltaTime, 0.0f, 1.0f);
            phys->angularVelocity *= damp;
        }

        // 2. Thu thap collider
        std::vector<ColliderComponent *> allColliders;
        CollectColliders(root, allColliders);

        std::set<std::pair<GameObject *, GameObject *>> currentTriggers;

        for (size_t i = 0; i < allColliders.size(); ++i)
        {
            for (size_t j = i + 1; j < allColliders.size(); ++j)
            {
                auto *a = allColliders[i];
                auto *b = allColliders[j];
                if (a->Owner() == b->Owner())
                    continue;

                CollisionResult res = TestPair(a, b);
                if (!res.colliding)
                    continue;

                bool anyTrigger = a->isTrigger || b->isTrigger;

                if (anyTrigger)
                {
                    GameObject *first = std::min(a->Owner(), b->Owner());
                    GameObject *second = std::max(a->Owner(), b->Owner());
                    currentTriggers.insert({first, second});
                    continue;
                }

                auto *physA = a->Owner()->GetComponent<PhysicsComponent>();
                auto *physB = b->Owner()->GetComponent<PhysicsComponent>();

                if (physA && !physA->isKinematic)
                {
                    a->Owner()->transform.position.x += res.normal.x * res.depth;
                    a->Owner()->transform.position.y += res.normal.y * res.depth;

                    ApplyRollingFriction(a->Owner(), physA, res.normal);

                    physA->velocity.x = 0.0f;
                    physA->velocity.y = 0.0f;
                }
                else if (physB && !physB->isKinematic)
                {
                    b->Owner()->transform.position.x -= res.normal.x * res.depth;
                    b->Owner()->transform.position.y -= res.normal.y * res.depth;

                    engine::math::Vector2 flippedNormal = {-res.normal.x, -res.normal.y};
                    ApplyRollingFriction(b->Owner(), physB, flippedNormal);

                    physB->velocity.x = 0.0f;
                    physB->velocity.y = 0.0f;
                }
            }
        }

        // 3. Va cham voi TilemapCollider (xu ly rieng vi khong ke thua ColliderComponent)
        std::vector<StaticBox> tilemapBoxes;
        CollectTilemapBoxes(root, tilemapBoxes);

        for (auto& tbox : tilemapBoxes) {
            for (auto* other : allColliders) {
                if (other->Owner() == tbox.owner) continue;

                CollisionResult res = TestStaticBoxVsCollider(tbox, other);
                if (!res.colliding) continue;

                bool anyTrigger = tbox.isTrigger || other->isTrigger;

                if (anyTrigger) {
                    GameObject* first = std::min(tbox.owner, other->Owner());
                    GameObject* second = std::max(tbox.owner, other->Owner());
                    currentTriggers.insert({first, second});
                    continue;
                }

                auto* phys = other->Owner()->GetComponent<PhysicsComponent>();
                if (phys && !phys->isKinematic) {
                    other->Owner()->transform.position.x += res.normal.x * res.depth;
                    other->Owner()->transform.position.y += res.normal.y * res.depth;
                    ApplyRollingFriction(other->Owner(), phys, res.normal);
                    phys->velocity.x = 0.0f;
                    phys->velocity.y = 0.0f;
                }
            }
        }

        for (auto &pair : currentTriggers)
        {
            if (s_activeTriggers.find(pair) == s_activeTriggers.end())
            {
                for (auto &comp : pair.first->Components())
                    comp->OnTriggerEnter(pair.second);
                for (auto &comp : pair.second->Components())
                    comp->OnTriggerEnter(pair.first);
            }
        }
        for (auto &pair : s_activeTriggers)
        {
            if (currentTriggers.find(pair) == currentTriggers.end())
            {
                for (auto &comp : pair.first->Components())
                    comp->OnTriggerExit(pair.second);
                for (auto &comp : pair.second->Components())
                    comp->OnTriggerExit(pair.first);
            }
        }
        s_activeTriggers = std::move(currentTriggers);
    }

} // namespace engine::core