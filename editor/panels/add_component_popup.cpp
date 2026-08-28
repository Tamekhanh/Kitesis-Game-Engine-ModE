#include "add_component_popup.h"
#include "engine/core/component_registry.h"
#include <imgui.h>

namespace editor
{

    void DrawAddComponentPopup(engine::core::GameObject &target)
    {
        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            for (auto &typeName : engine::core::ComponentRegistry::TypeNames())
            {
                if (ImGui::MenuItem(typeName.c_str()))
                {
                    engine::core::ComponentRegistry::Create(typeName, target);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }

} // namespace editor