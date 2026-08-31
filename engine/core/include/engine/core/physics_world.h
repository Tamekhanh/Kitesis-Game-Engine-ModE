#pragma once
#include "engine/core/game_object.h"
#include "engine/core/collider_component.h"
#include <set>
#include <utility>
#include <vector>

namespace engine::core {

class PhysicsWorld {
public:
    static void Step(GameObject& root, float deltaTime);
    static void Reset();

private:
    static void CollectBodies(GameObject& obj, std::vector<GameObject*>& out);
    static void CollectColliders(GameObject& obj, std::vector<ColliderComponent*>& out);

    static std::set<std::pair<GameObject*, GameObject*>> s_activeTriggers;
};

} // namespace engine::core