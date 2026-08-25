#pragma once
#include "engine/core/component.h"
#include "engine/core/game_object.h"
#include "engine/platform/input.h"

class MoveComponent : public engine::core::Component {
public:
    explicit MoveComponent(float speed) : m_speed(speed) {}

    void OnUpdate(float deltaTime) override {
        auto& pos = Owner()->transform.position;

        if (engine::platform::Input::IsKeyDown(engine::platform::Key::W)) pos.y -= m_speed * deltaTime;
        if (engine::platform::Input::IsKeyDown(engine::platform::Key::S)) pos.y += m_speed * deltaTime;
        if (engine::platform::Input::IsKeyDown(engine::platform::Key::A)) pos.x -= m_speed * deltaTime;
        if (engine::platform::Input::IsKeyDown(engine::platform::Key::D)) pos.x += m_speed * deltaTime;
    }

private:
    float m_speed;
};