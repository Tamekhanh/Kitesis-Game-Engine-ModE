#pragma once
#include "engine/core/component.h"
#include "engine/math/transform.h"
#include <vector>
#include <memory>
#include <string>
#include <type_traits>

namespace engine::core
{

    class GameObject
    {
    public:
        explicit GameObject(const std::string &name = "GameObject");
        ~GameObject();

        void SetName(const std::string &name) { m_name = name; }
        std::unique_ptr<GameObject> RemoveChild(GameObject *child);
        // --- Transform ---
        engine::math::Transform transform;
        engine::math::Vector2 GetWorldPosition() const;

        // --- Hierarchy ---
        GameObject *Parent() const { return m_parent; }
        const std::vector<std::unique_ptr<GameObject>> &Children() const { return m_children; }
        GameObject *AddChild(std::unique_ptr<GameObject> child);

        // --- Component ---
        template <typename T, typename... Args>
        T *AddComponent(Args &&...args)
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "T phai ke thua tu Component");
            auto comp = std::make_unique<T>(std::forward<Args>(args)...);
            T *raw = comp.get();
            comp->m_owner = this;
            m_components.push_back(std::move(comp));
            raw->OnStart();
            return raw;
        }

        template <typename T>
        T *GetComponent()
        {
            for (auto &c : m_components)
            {
                if (T *casted = dynamic_cast<T *>(c.get()))
                {
                    return casted;
                }
            }
            return nullptr;
        }

        template <typename T>
        std::vector<T *> GetComponents()
        {
            std::vector<T *> result;
            for (auto &c : m_components)
            {
                if (T *casted = dynamic_cast<T *>(c.get()))
                {
                    result.push_back(casted);
                }
            }
            return result;
        }

        // --- Lifecycle ---
        void Update(float deltaTime);
        void Render();
        void ClearChildren() { m_children.clear(); }
        const std::vector<std::unique_ptr<Component>> &Components() const { return m_components; }

        const std::string &Name() const { return m_name; }
        bool active = true;

    private:
        std::string m_name;
        GameObject *m_parent = nullptr;
        std::vector<std::unique_ptr<GameObject>> m_children;
        std::vector<std::unique_ptr<Component>> m_components;
    };

} // namespace engine::core
