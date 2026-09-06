#pragma once
#include "engine/core/component.h"
#include <string>
#include <memory>

namespace sol
{
    class state;
}

namespace engine::core
{

    class ScriptComponent : public Component
    {
    public:
        ScriptComponent();
        ~ScriptComponent();

        std::string ReadSourceCode() const;
        bool SaveSourceCode(const std::string &code);

        void LoadScript(const std::string &path);
        const std::string &ScriptPath() const { return m_scriptPath; }
        bool HasError() const { return m_hasError; }
        const std::string &ErrorMessage() const { return m_errorMessage; }

        void OnStart() override;
        void OnUpdate(float deltaTime) override;
        void OnTriggerEnter(GameObject *other) override;
        void OnTriggerExit(GameObject *other) override;

    private:
        void BindAPI();

        std::unique_ptr<sol::state> m_lua;
        std::string m_scriptPath;
        bool m_started = false;
        bool m_hasError = false;
        std::string m_errorMessage;
    };

} // namespace engine::core