#include "engine/core/component_registry.h"

namespace engine::core
{

    std::unordered_map<std::string, ComponentFactory> &ComponentRegistry::Map()
    {
        static std::unordered_map<std::string, ComponentFactory> instance;
        return instance;
    }

    std::vector<std::string> &ComponentRegistry::Names()
    {
        static std::vector<std::string> instance;
        return instance;
    }

    void ComponentRegistry::Register(const std::string &typeName, ComponentFactory factory)
    {
        Map()[typeName] = std::move(factory);
        Names().push_back(typeName);
    }

    bool ComponentRegistry::Create(const std::string &typeName, GameObject &target)
    {
        auto it = Map().find(typeName);
        if (it == Map().end())
            return false;
        it->second(target);
        return true;
    }

    const std::vector<std::string> &ComponentRegistry::TypeNames()
    {
        return Names();
    }

} // namespace engine::core