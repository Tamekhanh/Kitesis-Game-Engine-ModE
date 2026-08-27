#include "asset_browser_panel.h"
#include "engine/renderer/texture2d.h"
#include "engine/renderer/sprite_sheet_meta.h"
#include "sprite_editor_popup.h"
#include <imgui.h>
#include <vector>
#include <algorithm>
#include <sstream>

namespace editor {

AssetBrowserPanel::AssetBrowserPanel() = default;
AssetBrowserPanel::~AssetBrowserPanel() = default;

void AssetBrowserPanel::SetRootDirectory(const std::filesystem::path& root) {
    m_rootDir = root;
    m_currentDir = root;
    m_thumbnailCache.clear();
    m_expandedItems.clear();
}

static bool IsImageFile(const std::filesystem::path& path) {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg";
}

engine::renderer::Texture2D* AssetBrowserPanel::GetOrLoadThumbnail(const std::filesystem::path& path) {
    std::string key = path.string();
    auto it = m_thumbnailCache.find(key);
    if (it != m_thumbnailCache.end()) return it->second.get();

    auto texture = std::make_unique<engine::renderer::Texture2D>(key);
    engine::renderer::Texture2D* raw = texture.get();
    m_thumbnailCache[key] = std::move(texture);
    return raw;
}

// Ma hoa payload keo-tha 1 frame cu the thanh chuoi "duongdan|frameIndex"
static std::string EncodeFramePayload(const std::string& path, int frameIndex) {
    std::ostringstream oss;
    oss << path << "|" << frameIndex;
    return oss.str();
}

void AssetBrowserPanel::DrawExpandedFrames(const std::filesystem::path& path, int columns, int rows, float thumbSize) {
    auto* thumbnail = GetOrLoadThumbnail(path);
    if (!thumbnail || thumbnail->Id() == 0) return;

    float subSize = thumbSize * 0.6f;
    int totalFrames = columns * rows;
    std::string fullPath = path.string();

    ImGui::Indent(20.0f);
    int col = 0;
    int itemsPerRow = std::max(1, (int)((ImGui::GetContentRegionAvail().x) / (subSize + 8)));

    for (int i = 0; i < totalFrames; ++i) {
        int c = i % columns;
        int r = i / columns;

        float uMin = (float)c / (float)columns;
        float uMax = (float)(c + 1) / (float)columns;
        float vMin = (float)r / (float)rows;
        float vMax = (float)(r + 1) / (float)rows;

        // Lat V vi anh da bi lat luc load (giong moi cho khac trong project)
        ImVec2 uv0(uMin, vMax);
        ImVec2 uv1(uMax, vMin);

        ImGui::PushID(i);
        ImGui::BeginGroup();
        ImGui::Image((ImTextureID)(intptr_t)thumbnail->Id(), ImVec2(subSize, subSize), uv0, uv1);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            std::string payload = EncodeFramePayload(fullPath, i);
            ImGui::SetDragDropPayload("ASSET_IMAGE_FRAME", payload.c_str(), payload.size() + 1);
            ImGui::Text("Frame %d", i);
            ImGui::EndDragDropSource();
        }

        ImGui::Text("%d", i);
        ImGui::EndGroup();
        ImGui::PopID();

        col++;
        if (col < itemsPerRow) ImGui::SameLine(); else col = 0;
    }
    ImGui::Unindent(20.0f);
}

void AssetBrowserPanel::DrawImageItem(const std::filesystem::path& path, float itemWidth, float thumbSize) {
    std::string name = path.filename().string();
    std::string fullPath = path.string();

    auto meta = engine::renderer::SpriteSheetMeta::Load(path);
    bool isSpriteSheet = (meta.columns * meta.rows) > 1;
    bool isExpanded = m_expandedItems.count(fullPath) > 0;

    ImGui::PushID(name.c_str());
    ImGui::BeginGroup();

    // --- Mui ten mo rong, chi hien khi la sprite sheet ---
    if (isSpriteSheet) {
        const char* arrow = isExpanded ? "v" : ">";
        if (ImGui::SmallButton(arrow)) {
            if (isExpanded) m_expandedItems.erase(fullPath);
            else m_expandedItems.insert(fullPath);
        }
        ImGui::SameLine();
    } else {
        ImGui::Dummy(ImVec2(20.0f, 1.0f));
        ImGui::SameLine();
    }

    auto* thumbnail = GetOrLoadThumbnail(path);
    bool clicked = false;

    if (thumbnail && thumbnail->Id() != 0) {
        clicked = ImGui::ImageButton(
            name.c_str(),
            (ImTextureID)(intptr_t)thumbnail->Id(),
            ImVec2(thumbSize, thumbSize),
            ImVec2(0, 1), ImVec2(1, 0));
    } else {
        clicked = ImGui::Button("?", ImVec2(thumbSize, thumbSize));
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        OpenSpriteEditorForPath(path);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("ASSET_IMAGE_PATH", fullPath.c_str(), fullPath.size() + 1);
        ImGui::Text("%s", name.c_str());
        ImGui::EndDragDropSource();
    }

    ImGui::SameLine();
    if (isSpriteSheet) {
        ImGui::Text("%s  [%dx%d]", name.c_str(), meta.columns, meta.rows);
    } else {
        ImGui::Text("%s", name.c_str());
    }

    ImGui::EndGroup();

    // --- Danh sach frame con, chi ve khi dang mo rong ---
    if (isSpriteSheet && isExpanded) {
        DrawExpandedFrames(path, meta.columns, meta.rows, thumbSize);
    }

    ImGui::PopID();
}

void AssetBrowserPanel::Draw() {
    ImGui::Begin("Assets");

    if (m_rootDir.empty()) {
        ImGui::TextDisabled("Chua co project nao duoc mo");
        ImGui::End();
        return;
    }

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

    std::vector<std::filesystem::directory_entry> folders;
    std::vector<std::filesystem::directory_entry> files;

    for (auto& entry : std::filesystem::directory_iterator(m_currentDir)) {
        if (entry.is_directory()) folders.push_back(entry);
        else if (IsImageFile(entry.path())) files.push_back(entry);
    }

    for (auto& entry : folders) {
        std::string name = entry.path().filename().string();
        ImGui::PushID(name.c_str());
        if (ImGui::Button(("[DIR] " + name).c_str(), ImVec2(-1, 0))) {
            m_currentDir = entry.path();
        }
        ImGui::PopID();
    }

    if (!folders.empty()) ImGui::Separator();

    const float itemWidth = -1.0f; // full width, danh sach doc thay vi luoi
    const float thumbSize = 48.0f;

    for (auto& entry : files) {
        DrawImageItem(entry.path(), itemWidth, thumbSize);
    }

    ImGui::End();
}

} // namespace editor