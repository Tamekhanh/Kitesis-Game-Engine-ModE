
#include "engine/core/script_component.h"
#include "engine/core/game_object.h"
#include "engine/core/component_registry.h"
#include "engine/core/logger.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <fstream>
#include <sstream>

namespace engine::core
{

    ScriptComponent::ScriptComponent()
    {
        m_lua = std::make_unique<sol::state>();
        m_lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
        BindAPI();
    }

    ScriptComponent::~ScriptComponent() = default;

    void ScriptComponent::BindAPI()
    {
        // --- Vector2 ---
        m_lua->new_usertype<engine::math::Vector2>("Vector2",
                                                   sol::constructors<engine::math::Vector2(), engine::math::Vector2(float, float)>(),
                                                   "x", &engine::math::Vector2::x,
                                                   "y", &engine::math::Vector2::y);

        // --- GameObject (chi expose nhung gi can thiet, khong toan bo API C++) ---
        m_lua->new_usertype<GameObject>("GameObject",
                                        "name", sol::property(&GameObject::Name),
                                        "active", &GameObject::active);

        // --- Transform, truy cap qua "self.transform" ---
        m_lua->new_usertype<engine::math::Transform>("Transform",
                                                     "position", &engine::math::Transform::position,
                                                     "rotation", &engine::math::Transform::rotation,
                                                     "scale", &engine::math::Transform::scale);

        // --- Ham log tu Lua ---
        (*m_lua)["log"] = [](const std::string &msg)
        {
            Logger::Info("[Lua] " + msg);
        };

        // --- "self": chinh GameObject dang gan script nay, gan lai moi lan OnStart ---
    }

    std::string ScriptComponent::ReadSourceCode() const
    {
        if (m_scriptPath.empty())
            return "";
        std::ifstream file(m_scriptPath);
        if (!file.is_open())
            return "";
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    bool ScriptComponent::SaveSourceCode(const std::string &code)
    {
        if (m_scriptPath.empty())
            return false;
        std::ofstream out(m_scriptPath);
        if (!out.is_open())
            return false;
        out << code;
        out.close();
        return true;
    }

    void ScriptComponent::LoadScript(const std::string &path)
    {
        m_scriptPath = path;
        m_hasError = false;
        m_errorMessage.clear();

        std::ifstream file(path);
        if (!file.is_open())
        {
            m_hasError = true;
            m_errorMessage = "Khong the mo file: " + path;
            Logger::Error(m_errorMessage);
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        auto result = m_lua->safe_script(buffer.str(), sol::script_pass_on_error);
        if (!result.valid())
        {
            sol::error err = result;
            m_hasError = true;
            m_errorMessage = err.what();
            Logger::Error("[Lua] Loi script " + path + ": " + m_errorMessage);
        }
    }

    void ScriptComponent::OnStart()
    {
        if (m_hasError)
            return;

        (*m_lua)["self"] = Owner();
        (*m_lua)["transform"] = &Owner()->transform;

        sol::function onStart = (*m_lua)["onStart"];
        if (onStart.valid())
        {
            auto result = onStart();
            if (!result.valid())
            {
                sol::error err = result;
                Logger::Error("[Lua] Loi onStart: " + std::string(err.what()));
            }
        }
        m_started = true;
    }

    void ScriptComponent::OnUpdate(float deltaTime)
    {
        if (m_hasError || !m_started)
            return;

        sol::function onUpdate = (*m_lua)["onUpdate"];
        if (onUpdate.valid())
        {
            auto result = onUpdate(deltaTime);
            if (!result.valid())
            {
                sol::error err = result;
                Logger::Error("[Lua] Loi onUpdate: " + std::string(err.what()));
            }
        }
    }

    void ScriptComponent::OnTriggerEnter(GameObject *other)
    {
        if (m_hasError || !m_started)
            return;
        sol::function fn = (*m_lua)["onTriggerEnter"];
        if (fn.valid())
            fn(other);
    }

    void ScriptComponent::OnTriggerExit(GameObject *other)
    {
        if (m_hasError || !m_started)
            return;
        sol::function fn = (*m_lua)["onTriggerExit"];
        if (fn.valid())
            fn(other);
    }

    static ComponentAutoRegister s_scriptRegister(
        "ScriptComponent",
        [](GameObject &obj)
        {
            obj.AddComponent<ScriptComponent>();
        });

} // namespace engine::core