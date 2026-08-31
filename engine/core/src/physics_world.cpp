#include "engine/core/physics_world.h"
#include "engine/core/physics_component.h"
#include "engine/core/box_collider_component.h"
#include "engine/core/circle_collider_component.h"
#include "engine/core/tilemap_collider_component.h"
#include "engine/core/collision_math.h"
#include <algorithm>

namespace engine::core {

static const float kGravity = 980.0f;

std::set<std::pair<GameObject*, GameObject*>> PhysicsWorld::s_activeTriggers;

void PhysicsWorld::Reset() { s_activeTriggers.clear(); }

void PhysicsWorld::CollectBodies(GameObject& obj, std::vector<GameObject*>& out) {
    if (obj.GetComponent<PhysicsComponent>()) out.push_back(&obj);
    for (auto& child : obj.Children()) CollectBodies(*child, out);
}

void PhysicsWorld::CollectColliders(GameObject& obj, std::vector<ColliderComponent*>& out) {
    for (auto* c : obj.GetComponents<ColliderComponent>()) out.push_back(c);
    for (auto& child : obj.Children()) CollectColliders(*child, out);
}

static CollisionResult TestPair(ColliderComponent* a, ColliderComponent* b) {
    bool aIsBox = (a->Shape() == ColliderShape::Box);
    bool bIsBox = (b->Shape() == ColliderShape::Box);

    if (aIsBox && bIsBox) {
        return TestBoxBox(*static_cast<BoxColliderComponent*>(a), *static_cast<BoxColliderComponent*>(b));
    }
    if (!aIsBox && !bIsBox) {
        return TestCircleCircle(*static_cast<CircleColliderComponent*>(a), *static_cast<CircleColliderComponent*>(b));
    }
    if (aIsBox && !bIsBox) {
        auto res = TestBoxCircle(*static_cast<BoxColliderComponent*>(a), *static_cast<CircleColliderComponent*>(b));
        return res;
    }
    // !aIsBox && bIsBox: dao nguoc thu tu, roi dao lai normal
    auto res = TestBoxCircle(*static_cast<BoxColliderComponent*>(b), *static_cast<CircleColliderComponent*>(a));
    res.normal = { -res.normal.x, -res.normal.y };
    return res;
}

void PhysicsWorld::Step(GameObject& root, float deltaTime) {
    std::vector<GameObject*> bodies;
    CollectBodies(root, bodies);

    for (auto* body : bodies) {
        auto* phys = body->GetComponent<PhysicsComponent>();
        if (phys->useGravity && !phys->isKinematic) {
            phys->velocity.y += kGravity * phys->gravityScale * deltaTime;
        }
        body->transform.position.x += phys->velocity.x * deltaTime;
        body->transform.position.y += phys->velocity.y * deltaTime;
    }

    std::vector<ColliderComponent*> allColliders;
    CollectColliders(root, allColliders);

    std::set<std::pair<GameObject*, GameObject*>> currentTriggers;

    for (size_t i = 0; i < allColliders.size(); ++i) {
        for (size_t j = i + 1; j < allColliders.size(); ++j) {
            auto* a = allColliders[i];
            auto* b = allColliders[j];
            if (a->Owner() == b->Owner()) continue;

            CollisionResult res = TestPair(a, b);
            if (!res.colliding) continue;

            bool anyTrigger = a->isTrigger || b->isTrigger;

            if (anyTrigger) {
                GameObject* first = std::min(a->Owner(), b->Owner());
                GameObject* second = std::max(a->Owner(), b->Owner());
                currentTriggers.insert({first, second});
                continue;
            }

            auto* physA = a->Owner()->GetComponent<PhysicsComponent>();
            auto* physB = b->Owner()->GetComponent<PhysicsComponent>();

            if (physA && !physA->isKinematic) {
                a->Owner()->transform.position.x += res.normal.x * res.depth;
                a->Owner()->transform.position.y += res.normal.y * res.depth;
                physA->velocity.x = 0.0f;
                physA->velocity.y = 0.0f;
            } else if (physB && !physB->isKinematic) {
                b->Owner()->transform.position.x -= res.normal.x * res.depth;
                b->Owner()->transform.position.y -= res.normal.y * res.depth;
                physB->velocity.x = 0.0f;
                physB->velocity.y = 0.0f;
            }
        }
    }

    for (auto& pair : currentTriggers) {
        if (s_activeTriggers.find(pair) == s_activeTriggers.end()) {
            for (auto& comp : pair.first->Components())  comp->OnTriggerEnter(pair.second);
            for (auto& comp : pair.second->Components()) comp->OnTriggerEnter(pair.first);
        }
    }
    for (auto& pair : s_activeTriggers) {
        if (currentTriggers.find(pair) == currentTriggers.end()) {
            for (auto& comp : pair.first->Components())  comp->OnTriggerExit(pair.second);
            for (auto& comp : pair.second->Components()) comp->OnTriggerExit(pair.first);
        }
    }
    s_activeTriggers = std::move(currentTriggers);
}

} // namespace engine::core