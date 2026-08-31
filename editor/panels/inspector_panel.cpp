#include "inspector_panel.h"
#include "add_component_popup.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/project.h"
#include "engine/renderer/animation_component.h"
#include "engine/renderer/tilemap_component.h"
#include "engine/core/collider_component.h"
#include "engine/core/tilemap_collider_component.h"
#include "engine/renderer/tilemap_collider_baker.h"
#include "engine/core/physics_component.h"
#include "engine/core/box_collider_component.h"
#include "engine/core/circle_collider_component.h"

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

                ImGui::Spacing();
                ImGui::Text("Pivot");
                ImGui::DragFloat2("##pivot", &sprite.pivot.x, 0.01f, 0.0f, 1.0f);

                if (ImGui::SmallButton("Top-Left"))
                    sprite.pivot = {0.0f, 0.0f};
                ImGui::SameLine();
                if (ImGui::SmallButton("Center"))
                    sprite.pivot = {0.5f, 0.5f};
                ImGui::SameLine();
                if (ImGui::SmallButton("Bottom-Ctr"))
                    sprite.pivot = {0.5f, 1.0f};
            }
        }
    }

    static void DrawBoxColliderComponentUI(engine::core::BoxColliderComponent &collider)
    {
        if (ImGui::CollapsingHeader("Box Collider Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat2("Offset", &collider.offset.x, 1.0f);
            ImGui::DragFloat2("Size", &collider.size.x, 1.0f, 1.0f, 2000.0f);
            ImGui::Checkbox("Is Trigger", &collider.isTrigger);
        }
    }

    static void DrawCircleColliderComponentUI(engine::core::CircleColliderComponent &collider)
    {
        if (ImGui::CollapsingHeader("Circle Collider Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat2("Offset", &collider.offset.x, 1.0f);
            ImGui::DragFloat("Radius", &collider.radius, 1.0f, 1.0f, 1000.0f);
            ImGui::Checkbox("Is Trigger", &collider.isTrigger);
        }
    }

    static void DrawPhysicsComponentUI(engine::core::PhysicsComponent &phys)
    {
        if (ImGui::CollapsingHeader("Physics Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat2("Velocity", &phys.velocity.x, 1.0f);
            ImGui::Checkbox("Use Gravity", &phys.useGravity);
            ImGui::DragFloat("Gravity Scale", &phys.gravityScale, 0.1f, 0.0f, 10.0f);
            ImGui::Checkbox("Is Kinematic", &phys.isKinematic);
        }
    }

    static bool DrawTilemapComponentUI(engine::renderer::TilemapComponent &tilemap,
                                       engine::core::TilemapColliderComponent *existingCollider)
    {
        bool requestGenerate = false;

        if (ImGui::CollapsingHeader("Tilemap Component", ImGuiTreeNodeFlags_DefaultOpen))
        {

            std::string label = tilemap.TexturePath().empty()
                                    ? "Keo tileset tu Assets vao day"
                                    : tilemap.TexturePath();

            ImGui::Button(label.c_str(), ImVec2(-1, 40));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_PATH"))
                {
                    std::string droppedPath(static_cast<const char *>(payload->Data));
                    s_inspectorTextures.push_back(
                        std::make_unique<engine::renderer::Texture2D>(droppedPath));
                    tilemap.BindTileset(*s_inspectorTextures.back(), droppedPath);
                }
                ImGui::EndDragDropTarget();
            }

            if (!tilemap.TexturePath().empty())
            {
                ImGui::Text("Tileset grid: %d x %d", tilemap.TilesetColumns(), tilemap.TilesetRows());
            }

            ImGui::Spacing();
            ImGui::Separator();

            static int editWidth = tilemap.Width();
            static int editHeight = tilemap.Height();
            static engine::renderer::TilemapComponent *lastTilemap = nullptr;
            if (lastTilemap != &tilemap)
            {
                editWidth = tilemap.Width();
                editHeight = tilemap.Height();
                lastTilemap = &tilemap;
            }

            ImGui::DragInt("Grid Width", &editWidth, 1, 1, 200);
            ImGui::DragInt("Grid Height", &editHeight, 1, 1, 200);
            if (ImGui::Button("Resize Grid"))
            {
                tilemap.Resize(editWidth, editHeight);
                if (existingCollider)
                {
                    engine::renderer::BakeTilemapCollider(tilemap, *existingCollider);
                }
            }

            float tileSize = tilemap.TileSize();
            if (ImGui::DragFloat("Tile Size (px)", &tileSize, 1.0f, 1.0f, 512.0f))
            {
                tilemap.SetTileSize(tileSize);
                if (existingCollider)
                {
                    engine::renderer::BakeTilemapCollider(tilemap, *existingCollider);
                }
            }

            ImGui::Spacing();
            ImGui::Separator();

            if (!existingCollider)
            {
                if (ImGui::Button("Generate Collider"))
                {
                    requestGenerate = true;
                }
                ImGui::TextDisabled("Chua co Tilemap Collider Component");
            }
            else
            {
                ImGui::TextDisabled("Da co Tilemap Collider Component (xem khoi rieng ben duoi)");
            }
        }

        return requestGenerate;
    }

    static void DrawTilemapColliderComponentUI(engine::core::TilemapColliderComponent &tc)
    {
        if (ImGui::CollapsingHeader("Tilemap Collider Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("So hinh chu nhat da gop: %d", (int)tc.rects.size());
            ImGui::Checkbox("Is Trigger", &tc.isTrigger);
            ImGui::TextDisabled("Tu dong cap nhat khi ve/xoa tile (neu component nay ton tai)");
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

        bool wantGenerateTilemapCollider = false;
        engine::renderer::TilemapComponent *tilemapForBake = nullptr;
        engine::core::Component *pendingRemove = nullptr;

        for (auto &comp : obj->Components())
        {
            ImGui::PushID(comp.get());
            ImVec2 blockStart = ImGui::GetCursorScreenPos();

            if (auto *sprite = dynamic_cast<engine::renderer::SpriteComponent *>(comp.get()))
            {
                DrawSpriteComponentUI(*sprite);
            }
            if (dynamic_cast<engine::renderer::AnimationComponent *>(comp.get()))
            {
                ImGui::TextDisabled("Animation Component (xem tab 'Animation' de chinh sua)");
            }
            if (auto *tilemap = dynamic_cast<engine::renderer::TilemapComponent *>(comp.get()))
            {
                auto *existingCollider = obj->GetComponent<engine::core::TilemapColliderComponent>();
                if (DrawTilemapComponentUI(*tilemap, existingCollider))
                {
                    wantGenerateTilemapCollider = true;
                    tilemapForBake = tilemap;
                }
            }
            if (auto *box = dynamic_cast<engine::core::BoxColliderComponent *>(comp.get()))
            {
                DrawBoxColliderComponentUI(*box);
            }
            if (auto *circle = dynamic_cast<engine::core::CircleColliderComponent *>(comp.get()))
            {
                DrawCircleColliderComponentUI(*circle);
            }
            if (auto *tc = dynamic_cast<engine::core::TilemapColliderComponent *>(comp.get()))
            {
                DrawTilemapColliderComponentUI(*tc);
            }
            if (auto *phys = dynamic_cast<engine::core::PhysicsComponent *>(comp.get()))
            {
                DrawPhysicsComponentUI(*phys);
            }

            ImVec2 blockEnd = ImGui::GetCursorScreenPos();
            blockEnd.x = blockStart.x + ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorScreenPos(blockStart);
            ImGui::InvisibleButton("component_area", ImVec2(blockEnd.x - blockStart.x, blockEnd.y - blockStart.y));

            if (ImGui::BeginPopupContextItem("component_context"))
            {
                if (ImGui::MenuItem("Delete Component"))
                {
                    pendingRemove = comp.get();
                }
                ImGui::EndPopup();
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        if (pendingRemove)
        {
            obj->RemoveComponent(pendingRemove);
        }

        if (wantGenerateTilemapCollider && tilemapForBake)
        {
            auto *tilemapCollider = obj->GetComponent<engine::core::TilemapColliderComponent>();
            if (!tilemapCollider)
            {
                tilemapCollider = obj->AddComponent<engine::core::TilemapColliderComponent>();
            }
            engine::renderer::BakeTilemapCollider(*tilemapForBake, *tilemapCollider);
        }

        ImGui::Separator();
        DrawAddComponentPopup(*obj);

        ImGui::End();
    }

} // namespace editor