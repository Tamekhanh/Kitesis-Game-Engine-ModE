#include "engine/core/game_object.h"

namespace engine::core
{

    GameObject::GameObject(const std::string &name) : m_name(name) {}

    GameObject::~GameObject() = default;

    engine::math::Vector2 GameObject::GetWorldPosition() const
    {
        if (!m_parent)
        {
            return transform.position;
        }
        return m_parent->GetWorldPosition() + transform.position;
    }

    GameObject *GameObject::AddChild(std::unique_ptr<GameObject> child)
    {
        child->m_parent = this;
        GameObject *raw = child.get();
        m_children.push_back(std::move(child));
        return raw;
    }

    void GameObject::Update(float deltaTime)
    {
        if (!active)
            return;

        for (auto &comp : m_components)
        {
            comp->OnUpdate(deltaTime);
        }
        for (auto &child : m_children)
        {
            child->Update(deltaTime);
        }
    }

    void GameObject::Render()
    {
        if (!active)
            return;

        for (auto &comp : m_components)
        {
            comp->OnRender();
        }
        for (auto &child : m_children)
        {
            child->Render();
        }
    }

    std::unique_ptr<GameObject> GameObject::RemoveChild(GameObject *child)
    {
        for (auto it = m_children.begin(); it != m_children.end(); ++it)
        {
            if (it->get() == child)
            {
                std::unique_ptr<GameObject> removed = std::move(*it);
                m_children.erase(it);
                removed->m_parent = nullptr;
                return removed;
            }
        }
        return nullptr;
    }

} // namespace engine::core