#pragma once

namespace engine::core {

class GameObject;

class Component {
public:
    virtual ~Component() = default;

    // Gọi 1 lần khi component được gắn vào GameObject
    virtual void OnStart() {}

    // Gọi mỗi frame trước khi render
    virtual void OnUpdate(float deltaTime) {}

    // Gọi mỗi frame khi cần vẽ (component không cần vẽ thì bỏ qua override)
    virtual void OnRender() {}

    GameObject* Owner() const { return m_owner; }

private:
    friend class GameObject;
    GameObject* m_owner = nullptr;
};

} // namespace engine::core