#include "tile_palette_panel.h"
#include "engine/renderer/tilemap_component.h"
#include <imgui.h>
#include <algorithm>

namespace editor
{

    void DrawTilePalettePanel(EditorState &state)
    {
        ImGui::Begin("Tile Palette");

        if (!state.selectedObject)
        {
            ImGui::TextDisabled("Chua chon GameObject nao");
            ImGui::End();
            return;
        }

        auto *tilemap = state.selectedObject->GetComponent<engine::renderer::TilemapComponent>();
        if (!tilemap)
        {
            ImGui::TextDisabled("GameObject nay chua co TilemapComponent");
            ImGui::End();
            return;
        }

        if (tilemap->TexturePath().empty())
        {
            ImGui::TextDisabled("Tilemap chua co tileset");
            ImGui::End();
            return;
        }

        int columns = tilemap->TilesetColumns();
        int rows = tilemap->TilesetRows();
        int total = columns * rows;

        ImGui::Text("Tileset: %d x %d", columns, rows);
        ImGui::Separator();

        float thumbSize = 40.0f;
        int itemsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (thumbSize + 6)));
        unsigned int texId = tilemap->PreviewTextureId();

        int col = 0;
        for (int i = 0; i < total; ++i)
        {
            int c = i % columns;
            int r = i / columns;
            float uMin = (float)c / columns, uMax = (float)(c + 1) / columns;
            float vMin = (float)r / rows, vMax = (float)(r + 1) / rows;
            ImVec2 uv0(uMin, vMax), uv1(uMax, vMin);

            bool isSelected = (state.selectedTileIndex == i);

            ImGui::PushID(i);
            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.7f, 0.0f, 0.7f));
            }

            if (texId != 0)
            {
                if (ImGui::ImageButton("##t", (ImTextureID)(intptr_t)texId, ImVec2(thumbSize, thumbSize), uv0, uv1))
                {
                    state.selectedTileIndex = i;
                }
            }
            else
            {
                if (ImGui::Button("?", ImVec2(thumbSize, thumbSize)))
                {
                    state.selectedTileIndex = i;
                }
            }

            if (isSelected)
            {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();

            col++;
            if (col < itemsPerRow)
                ImGui::SameLine();
            else
                col = 0;
        }

        ImGui::NewLine();
        ImGui::Separator();
        if (state.selectedTileIndex >= 0)
        {
            ImGui::Text("Dang cam: Tile #%d", state.selectedTileIndex);
        }
        else
        {
            ImGui::TextDisabled("Chua chon tile nao (click vao mot o o tren)");
        }

        ImGui::End();
    }

} // namespace editor