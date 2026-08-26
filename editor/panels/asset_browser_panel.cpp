#include "asset_browser_panel.h"
#include <imgui.h>
#include <vector>
#include <algorithm>

namespace editor {

AssetBrowserPanel::AssetBrowserPanel() = default;

void AssetBrowserPanel::SetRootDirectory(const std::filesystem::path& root) {
    m_rootDir = root;
    m_currentDir = root;
}

static bool IsImageFile(const std::filesystem::path& path) {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

void AssetBrowserPanel::Draw() {
    ImGui::Begin("Assets");

    if (m_rootDir.empty()) {
        ImGui::TextDisabled("Chua co project nao duoc mo");
        ImGui::End();
        return;
    }

    // --- Thanh dieu huong: nut Back + duong dan hien tai ---
    bool canGoBack = (m_currentDir != m_rootDir);
    ImGui::BeginDisabled(!canGoBack);
    if (ImGui::Button("<- Back")) {
        m_currentDir = m_currentDir.parent_path();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    auto relative = std::filesystem::relative(m_currentDir, m_rootDir.parent_path());
    ImGui::Text("%s", relative.string().c_str());

    ImGui::Separator();

    // --- Liet ke thu muc con truoc, roi den file ---
    std::vector<std::filesystem::directory_entry> folders;
    std::vector<std::filesystem::directory_entry> files;

    for (auto& entry : std::filesystem::directory_iterator(m_currentDir)) {
        if (entry.is_directory()) {
            folders.push_back(entry);
        } else if (IsImageFile(entry.path())) {
            files.push_back(entry);
        }
    }

    const float itemWidth = 90.0f;
    int columns = std::max(1, (int)(ImGui::GetContentRegionAvail().x / itemWidth));
    int col = 0;

    for (auto& entry : folders) {
        std::string name = entry.path().filename().string();

        ImGui::PushID(name.c_str());
        if (ImGui::Button(("[DIR] " + name).c_str(), ImVec2(itemWidth - 10, 60))) {
            m_currentDir = entry.path();
        }
        ImGui::PopID();

        col++;
        if (col < columns) ImGui::SameLine();
        else col = 0;
    }

    for (auto& entry : files) {
        std::string name = entry.path().filename().string();
        std::string fullPath = entry.path().string();

        ImGui::PushID(name.c_str());
        ImGui::Button(name.c_str(), ImVec2(itemWidth - 10, 60));

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_IMAGE_PATH", fullPath.c_str(), fullPath.size() + 1);
            ImGui::Text("%s", name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PopID();

        col++;
        if (col < columns) ImGui::SameLine();
        else col = 0;
    }

    ImGui::End();
}

} // namespace editor