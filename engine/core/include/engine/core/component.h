#pragma once

namespace engine::core
{

    class GameObject;

    class Component
    {
    public:
        virtual ~Component() = default;

        virtual void OnStart() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}

        // Duoc PhysicsWorld goi khi Collider nay la Trigger va vua cham/roi khoi collider khac
        virtual void OnTriggerEnter(GameObject *other) {}
        virtual void OnTriggerExit(GameObject *other) {}

        GameObject *Owner() const { return m_owner; }

    private:
        friend class GameObject;
        GameObject *m_owner = nullptr;
    };

} // namespace engine::core