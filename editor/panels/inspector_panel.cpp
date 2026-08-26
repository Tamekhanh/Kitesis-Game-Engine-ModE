#include "inspector_panel.h"
#include <imgui.h>

namespace editor {

void DrawInspectorPanel(EditorState& state) {
    ImGui::Begin("Inspector");

    if (!state.selectedObject) {
        ImGui::TextDisabled("Chua chon GameObject nao");
        ImGui::End();
        return;
    }

    auto* obj = state.selectedObject;

    // --- Ten object, cho sua truc tiep ---
    static char nameBuffer[128];
    static engine::core::GameObject* lastEdited = nullptr;
    if (lastEdited != obj) {
        std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", obj->Name().c_str());
        lastEdited = obj;
    }

    ImGui::Text("Name: %s", obj->Name().c_str());
    ImGui::Checkbox("Active", &obj->active);

    ImGui::Separator();
    ImGui::Text("Transform");

    ImGui::DragFloat2("Position", &obj->transform.position.x, 1.0f);
    ImGui::DragFloat("Rotation", &obj->transform.rotation, 1.0f, -360.0f, 360.0f);
    ImGui::DragFloat2("Scale", &obj->transform.scale.x, 0.01f, 0.01f, 10.0f);

    ImGui::End();
}

} // namespace editor