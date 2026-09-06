#include "hierarchy_panel.h"
#include "engine/core/script_component.h"
#include <imgui.h>
#include <cstdio>
#include <string>

namespace editor
{

    static char s_renameBuffer[128];
    static engine::core::GameObject *s_renamingObject = nullptr;
    static bool s_focusRenameInput = false;

    struct PendingReparent
    {
        engine::core::GameObject *dragged = nullptr;
        engine::core::GameObject *newParent = nullptr;
    };

    static bool IsDescendant(engine::core::GameObject *candidate, engine::core::GameObject *obj)
    {
        if (!candidate)
            return false;
        if (candidate == obj)
            return true;
        for (auto &child : obj->Children())
        {
            if (IsDescendant(candidate, child.get()))
                return true;
        }
        return false;
    }

    static void DrawNode(engine::core::GameObject &obj, EditorState &state,
                         engine::core::GameObject *&pendingDelete,
                         PendingReparent &pendingReparent)
    {
        ImGui::PushID(&obj);

        bool isRenaming = (s_renamingObject == &obj);

        if (isRenaming)
        {
            if (s_focusRenameInput)
            {
                ImGui::SetKeyboardFocusHere();
                s_focusRenameInput = false;
            }
            ImGui::SetNextItemWidth(-1);
            bool confirmed = ImGui::InputText("##rename", s_renameBuffer, sizeof(s_renameBuffer),
                                              ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
            if (confirmed)
            {
                obj.SetName(s_renameBuffer);
                s_renamingObject = nullptr;
            }
            else if (ImGui::IsItemDeactivated())
            {
                s_renamingObject = nullptr;
            }
            ImGui::PopID();
            return;
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (state.selectedObject == &obj)
            flags |= ImGuiTreeNodeFlags_Selected;
        if (obj.Children().empty())
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool opened = ImGui::TreeNodeEx(obj.Name().c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            state.SelectGameObject(&obj);
        }
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
        {
            s_renamingObject = &obj;
            s_focusRenameInput = true;
            std::snprintf(s_renameBuffer, sizeof(s_renameBuffer), "%s", obj.Name().c_str());
        }

        if (ImGui::BeginDragDropSource())
        {
            engine::core::GameObject *ptr = &obj;
            ImGui::SetDragDropPayload("HIERARCHY_GAMEOBJECT", &ptr, sizeof(engine::core::GameObject *));
            ImGui::Text("%s", obj.Name().c_str());
            ImGui::EndDragDropSource();
        }

        // --- Chi GHI NHO yeu cau reparent, KHONG sua cay ngay tai day ---
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
            {
                auto *dragged = *static_cast<engine::core::GameObject **>(payload->Data);
                pendingReparent.dragged = dragged;
                pendingReparent.newParent = &obj;
            }
            else if (const ImGuiPayload *filePayload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
            {
                std::string droppedPath(static_cast<const char *>(filePayload->Data));
                if (droppedPath.size() >= 4 && droppedPath.substr(droppedPath.size() - 4) == ".lua")
                {
                    auto *script = obj.AddComponent<engine::core::ScriptComponent>();
                    script->LoadScript(droppedPath);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Rename"))
            {
                s_renamingObject = &obj;
                s_focusRenameInput = true;
                std::snprintf(s_renameBuffer, sizeof(s_renameBuffer), "%s", obj.Name().c_str());
            }
            if (ImGui::MenuItem("Delete"))
            {
                pendingDelete = &obj;
            }
            ImGui::EndPopup();
        }

        if (opened && !obj.Children().empty())
        {
            for (auto &child : obj.Children())
            {
                DrawNode(*child, state, pendingDelete, pendingReparent);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void DrawHierarchyPanel(engine::core::GameObject &root, EditorState &state)
    {
        ImGui::Begin("Hierarchy");

        if (ImGui::Button("+ Create Empty"))
        {
            auto newObj = std::make_unique<engine::core::GameObject>("GameObject");
            engine::core::GameObject *raw = root.AddChild(std::move(newObj));
            state.SelectGameObject(raw);
        }
        ImGui::Separator();

        engine::core::GameObject *pendingDelete = nullptr;
        PendingReparent pendingReparent;

        for (auto &child : root.Children())
        {
            DrawNode(*child, state, pendingDelete, pendingReparent);
        }

        ImGui::Dummy(ImGui::GetContentRegionAvail());
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
            {
                auto *dragged = *static_cast<engine::core::GameObject **>(payload->Data);
                pendingReparent.dragged = dragged;
                pendingReparent.newParent = &root;
            }
            ImGui::EndDragDropTarget();
        }

        // --- Xu ly THAT SU sau khi toan bo cay da ve xong ---
        if (pendingReparent.dragged && pendingReparent.newParent)
        {
            auto *dragged = pendingReparent.dragged;
            auto *newParent = pendingReparent.newParent;

            bool valid = (dragged != newParent) &&
                         !IsDescendant(newParent, dragged) &&
                         dragged->Parent() != nullptr &&
                         dragged->Parent() != newParent;

            if (valid)
            {
                engine::core::GameObject *oldParent = dragged->Parent();
                auto owned = oldParent->RemoveChild(dragged);
                if (owned)
                {
                    newParent->AddChild(std::move(owned));
                }
            }
        }

        if (pendingDelete)
        {
            bool selectedAffected = state.selectedObject &&
                                    (state.selectedObject == pendingDelete || IsDescendant(state.selectedObject, pendingDelete));
            if (selectedAffected)
            {
                state.SelectGameObject(nullptr);
            }
            engine::core::GameObject *parent = pendingDelete->Parent();
            if (parent)
            {
                parent->RemoveChild(pendingDelete);
            }
        }

        ImGui::End();
    }

} // namespace editor