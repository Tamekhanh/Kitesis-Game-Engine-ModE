#pragma once
#include "engine/core/game_object.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace engine::core
{

    using ComponentFactory = std::function<void(GameObject &)>;

    class ComponentRegistry
    {
    public:
        static void Register(const std::string &typeName, ComponentFactory factory);
        static bool Create(const std::string &typeName, GameObject &target);
        static const std::vector<std::string> &TypeNames();

    private:
        static std::unordered_map<std::string, ComponentFactory> &Map();
        static std::vector<std::string> &Names();
    };

    // Giup dang ky 1 dong duy nhat trong file .cpp cua tung Component
    struct ComponentAutoRegister
    {
        ComponentAutoRegister(const std::string &typeName, ComponentFactory factory)
        {
            ComponentRegistry::Register(typeName, std::move(factory));
        }
    };

} // namespace engine::core