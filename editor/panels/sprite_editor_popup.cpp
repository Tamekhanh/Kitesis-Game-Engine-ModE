#include "sprite_editor_popup.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/sprite_sheet_meta.h"
#include "engine/renderer/texture2d.h"
#include <imgui.h>
#include <memory>
#include <algorithm>

static void ToDisplayUV(float uMin, float vMin, float uMax, float vMax,
                        ImVec2 &outUV0, ImVec2 &outUV1)
{
    outUV0 = ImVec2(uMin, vMax);
    outUV1 = ImVec2(uMax, vMin);
}

namespace editor
{

    static bool s_open = false;
    static std::filesystem::path s_targetPath;
    static engine::renderer::SpriteComponent *s_targetComponent = nullptr;
    static std::unique_ptr<engine::renderer::Texture2D> s_previewTexture;
    static int s_columns = 1;
    static int s_rows = 1;

    static void LoadPreview(const std::filesystem::path &path)
    {
        s_previewTexture = std::make_unique<engine::renderer::Texture2D>(path.string());
        auto meta = engine::renderer::SpriteSheetMeta::Load(path);
        s_columns = meta.columns;
        s_rows = meta.rows;
    }

    void OpenSpriteEditorForPath(const std::filesystem::path &texturePath)
    {
        s_open = true;
        s_targetPath = texturePath;
        s_targetComponent = nullptr;
        LoadPreview(texturePath);
    }

    void OpenSpriteEditorForComponent(engine::renderer::SpriteComponent &sprite)
    {
        s_open = true;
        s_targetPath = sprite.TexturePath();
        s_targetComponent = &sprite;
        LoadPreview(s_targetPath);
    }

    void DrawSpriteEditorPopup()
    {
        if (!s_open)
            return;

        ImGui::OpenPopup("Sprite Editor");
        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowSize(ImVec2(480, 580), ImGuiCond_FirstUseEver);

        if (ImGui::BeginPopupModal("Sprite Editor", &s_open))
        {
            ImGui::Text("File: %s", s_targetPath.filename().string().c_str());
            ImGui::Separator();

            ImGui::DragInt("Columns", &s_columns, 1, 1, 32);
            ImGui::DragInt("Rows", &s_rows, 1, 1, 32);

            ImGui::Spacing();

            float previewSize = std::min(ImGui::GetContentRegionAvail().x, 400.0f);
            ImVec2 imagePos = ImGui::GetCursorScreenPos();

            if (s_previewTexture)
            {
                ImGui::Image(
                    (ImTextureID)(intptr_t)s_previewTexture->Id(),
                    ImVec2(previewSize, previewSize),
                    ImVec2(0, 1), ImVec2(1, 0));
            }

            ImDrawList *drawList = ImGui::GetWindowDrawList();
            float cellW = previewSize / (float)s_columns;
            float cellH = previewSize / (float)s_rows;

            for (int c = 1; c < s_columns; ++c)
            {
                float x = imagePos.x + c * cellW;
                drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + previewSize),
                                  IM_COL32(255, 255, 255, 150));
            }
            for (int r = 1; r < s_rows; ++r)
            {
                float y = imagePos.y + r * cellH;
                drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + previewSize, y),
                                  IM_COL32(255, 255, 255, 150));
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Danh sach frame:");

            if (s_previewTexture)
            {
                float listThumbSize = 48.0f;
                int totalFrames = s_columns * s_rows;
                int itemsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (listThumbSize + 8)));

                for (int i = 0; i < totalFrames; ++i)
                {
                    int col = i % s_columns;
                    int row = i / s_columns;

                    float uMin = (float)col / (float)s_columns;
                    float uMax = (float)(col + 1) / (float)s_columns;
                    float vMin = (float)row / (float)s_rows;
                    float vMax = (float)(row + 1) / (float)s_rows;

                    ImVec2 uv0, uv1;
                    ToDisplayUV(uMin, vMin, uMax, vMax, uv0, uv1);

                    ImGui::PushID(i);
                    ImGui::BeginGroup();
                    ImGui::Image(
                        (ImTextureID)(intptr_t)s_previewTexture->Id(),
                        ImVec2(listThumbSize, listThumbSize),
                        uv0, uv1);
                    ImGui::Text("%d", i);
                    ImGui::EndGroup();
                    ImGui::PopID();

                    if ((i + 1) % itemsPerRow != 0 && i != totalFrames - 1)
                    {
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::Spacing();
            ImGui::Text("Tong so frame: %d", s_columns * s_rows);
            ImGui::Separator();

            if (ImGui::Button("Apply", ImVec2(120, 0)))
            {
                engine::renderer::SpriteSheetMeta meta;
                meta.columns = s_columns;
                meta.rows = s_rows;
                engine::renderer::SpriteSheetMeta::Save(s_targetPath, meta);

                if (s_targetComponent)
                {
                    s_targetComponent->SetGrid(s_columns, s_rows);
                    s_targetComponent->SetFrame(0);
                }

                s_open = false;
                s_previewTexture.reset();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                s_open = false;
                s_previewTexture.reset();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

} // namespace editor