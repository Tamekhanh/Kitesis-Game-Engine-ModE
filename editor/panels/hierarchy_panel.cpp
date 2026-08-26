#include "hierarchy_panel.h"
#include <imgui.h>

namespace editor
{

    static void DrawNode(engine::core::GameObject &obj, EditorState &state)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (state.selectedObject == &obj)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (obj.Children().empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        // Dùng con trỏ object làm ID duy nhất, tách khỏi tên hiển thị (có thể trùng)
        ImGui::PushID(&obj);
        bool opened = ImGui::TreeNodeEx(obj.Name().c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            state.selectedObject = &obj;
        }

        if (opened && !obj.Children().empty())
        {
            for (auto &child : obj.Children())
            {
                DrawNode(*child, state);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void DrawHierarchyPanel(engine::core::GameObject &root, EditorState &state)
    {
        ImGui::Begin("Hierarchy");

        for (auto &child : root.Children())
        {
            DrawNode(*child, state);
        }

        ImGui::End();
    }

} // namespace editor