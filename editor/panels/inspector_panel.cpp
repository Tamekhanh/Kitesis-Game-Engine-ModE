#include "inspector_panel.h"
#include "add_component_popup.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/project.h"
#include "engine/renderer/animation_component.h"
#include "sprite_editor_popup.h"
#include <imgui.h>
#include <memory>
#include <vector>
#include <sstream>
namespace editor
{

    // Kho tam giu texture nguoi dung gan qua Inspector, tranh huy som
    static std::vector<std::unique_ptr<engine::renderer::Texture2D>> s_inspectorTextures;

    static void DrawSpriteComponentUI(engine::renderer::SpriteComponent &sprite)
    {
        if (ImGui::CollapsingHeader("Sprite Component", ImGuiTreeNodeFlags_DefaultOpen))
        {

            std::string label = sprite.TexturePath().empty()
                                    ? "Keo anh tu Assets vao day"
                                    : sprite.TexturePath();

            ImGui::Button(label.c_str(), ImVec2(-1, 40));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_PATH"))
                {
                    std::string droppedPath(static_cast<const char *>(payload->Data));
                    s_inspectorTextures.push_back(
                        std::make_unique<engine::renderer::Texture2D>(droppedPath));
                    sprite.BindTexture(*s_inspectorTextures.back(), droppedPath);
                }
                else if (const ImGuiPayload *framePayload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_FRAME"))
                {
                    std::string encoded(static_cast<const char *>(framePayload->Data));
                    size_t sep = encoded.rfind('|');
                    if (sep != std::string::npos)
                    {
                        std::string path = encoded.substr(0, sep);
                        int frameIndex = std::stoi(encoded.substr(sep + 1));

                        s_inspectorTextures.push_back(
                            std::make_unique<engine::renderer::Texture2D>(path));
                        sprite.BindTexture(*s_inspectorTextures.back(), path);
                        sprite.SetFrame(frameIndex);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::TreeNode("Nhap duong dan thu cong"))
            {
                static char pathBuffer[256] = "sprite.png";
                ImGui::InputText("##texpath", pathBuffer, sizeof(pathBuffer));
                ImGui::SameLine();
                if (ImGui::Button("Load"))
                {
                    std::string fullPath = engine::core::Project::AssetPath(pathBuffer).string();
                    s_inspectorTextures.push_back(
                        std::make_unique<engine::renderer::Texture2D>(fullPath));
                    sprite.BindTexture(*s_inspectorTextures.back(), fullPath);
                }
                ImGui::TreePop();
            }

            if (!sprite.TexturePath().empty())
            {
                ImGui::Spacing();

                // Preview nho, cung lat UV dung
                float thumbSize = 80.0f;
                ImGui::Image(
                    (ImTextureID)(intptr_t)sprite.PreviewTextureId(),
                    ImVec2(thumbSize, thumbSize),
                    ImVec2(0, 1), ImVec2(1, 0));

                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("Grid: %d x %d", sprite.Columns(), sprite.Rows());
                ImGui::EndGroup();
            }
        }
    }

    void DrawInspectorPanel(EditorState &state)
    {
        ImGui::Begin("Inspector");

        if (!state.selectedObject)
        {
            ImGui::TextDisabled("Chua chon GameObject nao");
            ImGui::End();
            return;
        }

        auto *obj = state.selectedObject;

        ImGui::Text("Name: %s", obj->Name().c_str());
        ImGui::Checkbox("Active", &obj->active);

        ImGui::Separator();
        ImGui::Text("Transform");

        ImGui::DragFloat2("Position", &obj->transform.position.x, 1.0f);
        ImGui::DragFloat("Rotation", &obj->transform.rotation, 1.0f, -360.0f, 360.0f);
        ImGui::DragFloat2("Scale", &obj->transform.scale.x, 0.01f, 0.01f, 10.0f);

        ImGui::Separator();

        engine::renderer::SpriteComponent *spriteComp = obj->GetComponent<engine::renderer::SpriteComponent>();

        for (auto &comp : obj->Components())
        {
            if (auto *sprite = dynamic_cast<engine::renderer::SpriteComponent *>(comp.get()))
            {
                DrawSpriteComponentUI(*sprite);
            }
            if (dynamic_cast<engine::renderer::AnimationComponent *>(comp.get()))
            {
                ImGui::TextDisabled("Animation Component (xem tab 'Animation' de chinh sua)");
            }
        }

        ImGui::Separator();
        DrawAddComponentPopup(*obj);

        ImGui::End();
    }

} // namespace editor