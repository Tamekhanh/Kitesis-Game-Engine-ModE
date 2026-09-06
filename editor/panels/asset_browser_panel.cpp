#include "asset_browser_panel.h"
#include "engine/renderer/texture2d.h"
#include "engine/renderer/sprite_sheet_meta.h"
#include "engine/core/logger.h"
#include "sprite_editor_popup.h"
#include <imgui.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <sstream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#endif

namespace editor {

static char s_renameBuffer[260];
static std::string s_renamingPath;
static bool s_focusRenameInput = false;
static std::string s_pendingDeletePath;
static bool s_showDeleteConfirm = false;

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

static const char* FileTypeLabel(const std::filesystem::path& path) {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".lua") return "[LUA]";
    if (ext == ".kiscene") return "[SCENE]";
    return "[FILE]";
}

static void OpenFolderInExplorer(const std::filesystem::path& folder) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", folder.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#endif
}

static void SelectFileInExplorer(const std::filesystem::path& file) {
#ifdef _WIN32
    std::string arg = "/select,\"" + file.string() + "\"";
    ShellExecuteA(nullptr, "open", "explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
#endif
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

static std::string EncodeFramePayload(const std::string& path, int frameIndex) {
    return path + "|" + std::to_string(frameIndex);
}

void AssetBrowserPanel::DrawExpandedFrames(const std::filesystem::path& path, int columns, int rows, float thumbSize) {
    auto* thumbnail = GetOrLoadThumbnail(path);
    if (!thumbnail || thumbnail->Id() == 0) return;

    float subSize = thumbSize * 0.6f;
    int totalFrames = columns * rows;
    std::string fullPath = path.string();

    ImGui::Indent(20.0f);
    int col = 0;
    int itemsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (subSize + 8)));

    for (int i = 0; i < totalFrames; ++i) {
        int c = i % columns;
        int r = i / columns;
        float uMin = (float)c / columns, uMax = (float)(c + 1) / columns;
        float vMin = (float)r / rows, vMax = (float)(r + 1) / rows;
        ImVec2 uv0(uMin, vMax), uv1(uMax, vMin);

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

void AssetBrowserPanel::DrawImageItem(const std::filesystem::path& path, EditorState& state) {
    // Ve rieng phan thumbnail anh + expand sprite sheet, roi giao lai cho DrawEntry xu ly
    // rename/delete/drag chung - o day chi tra ve, khong tu Begin/End gi ca (goi tu DrawEntry)
    std::string fullPath = path.string();
    auto meta = engine::renderer::SpriteSheetMeta::Load(path);
    bool isSpriteSheet = (meta.columns * meta.rows) > 1;
    bool isExpanded = m_expandedItems.count(fullPath) > 0;
    float thumbSize = 48.0f;

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
    if (thumbnail && thumbnail->Id() != 0) {
        ImGui::ImageButton(path.filename().string().c_str(),
            (ImTextureID)(intptr_t)thumbnail->Id(), ImVec2(thumbSize, thumbSize),
            ImVec2(0, 1), ImVec2(1, 0));
    } else {
        ImGui::Button("?", ImVec2(thumbSize, thumbSize));
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("ASSET_FILE_PATH", fullPath.c_str(), fullPath.size() + 1);
        ImGui::Text("%s", path.filename().string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemClicked() && !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        state.SelectAsset(path);
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        OpenSpriteEditorForPath(path);
    }

    if (isSpriteSheet && isExpanded) {
        DrawExpandedFrames(path, meta.columns, meta.rows, thumbSize);
    }
}

void AssetBrowserPanel::DrawEntry(const std::filesystem::path& path, bool isDir, EditorState& state) {
    std::string name = path.filename().string();
    std::string fullPath = path.string();
    bool isRenaming = (s_renamingPath == fullPath);

    ImGui::PushID(fullPath.c_str());

    if (isRenaming) {
        if (s_focusRenameInput) {
            ImGui::SetKeyboardFocusHere();
            s_focusRenameInput = false;
        }
        ImGui::SetNextItemWidth(-1);
        bool confirmed = ImGui::InputText("##rename", s_renameBuffer, sizeof(s_renameBuffer),
                                           ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
        if (confirmed) {
            std::filesystem::path newPath = path.parent_path() / s_renameBuffer;
            std::error_code ec;
            std::filesystem::rename(path, newPath, ec);
            if (ec) {
                engine::core::Logger::Error("Doi ten that bai: " + ec.message());
            }
            s_renamingPath.clear();
        } else if (ImGui::IsItemDeactivated()) {
            s_renamingPath.clear();
        }
        ImGui::PopID();
        return;
    }

    if (isDir) {
        if (ImGui::Button(("[DIR] " + name).c_str(), ImVec2(-1, 0))) {
            m_currentDir = path;
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_FILE_PATH", fullPath.c_str(), fullPath.size() + 1);
            ImGui::Text("%s", name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH")) {
                std::string droppedPath(static_cast<const char*>(payload->Data));
                std::filesystem::path src(droppedPath);
                std::filesystem::path dst = path / src.filename();
                if (src != dst && src != path) {
                    std::error_code ec;
                    std::filesystem::rename(src, dst, ec);
                    if (ec) {
                        engine::core::Logger::Error("Di chuyen that bai: " + ec.message());
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    } else if (IsImageFile(path)) {
        DrawImageItem(path, state);
        ImGui::SameLine();
        ImGui::Text("%s", name.c_str());
    } else {
        ImGui::Dummy(ImVec2(20.0f, 1.0f));
        ImGui::SameLine();
        ImGui::Button(FileTypeLabel(path), ImVec2(60, 30));

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_FILE_PATH", fullPath.c_str(), fullPath.size() + 1);
            ImGui::Text("%s", name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::IsItemClicked() && !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            state.SelectAsset(path);
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
    #ifdef _WIN32
            ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    #endif
        }
        ImGui::SameLine();
        ImGui::Text("%s", name.c_str());
    }

    // --- Chuot phai: Rename / Delete / Show in Explorer ---
    if (ImGui::BeginPopupContextItem("entry_context")) {
        if (ImGui::MenuItem("Rename")) {
            s_renamingPath = fullPath;
            s_focusRenameInput = true;
            std::snprintf(s_renameBuffer, sizeof(s_renameBuffer), "%s", name.c_str());
        }
        if (ImGui::MenuItem("Delete")) {
            s_pendingDeletePath = fullPath;
            s_showDeleteConfirm = true;
        }
        if (isDir) {
            if (ImGui::MenuItem("Open in Explorer")) {
                OpenFolderInExplorer(path);
            }
        } else {
            if (ImGui::MenuItem("Show in Explorer")) {
                SelectFileInExplorer(path);
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void AssetBrowserPanel::DrawDeleteConfirmPopup() {
    if (!s_showDeleteConfirm) return;

    ImGui::OpenPopup("Xac nhan xoa");
    if (ImGui::BeginPopupModal("Xac nhan xoa", &s_showDeleteConfirm, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Xoa vinh vien:\n%s", s_pendingDeletePath.c_str());
        ImGui::Text("Hanh dong nay khong the hoan tac.");
        ImGui::Separator();

        if (ImGui::Button("Xoa", ImVec2(120, 0))) {
            std::error_code ec;
            std::filesystem::remove_all(s_pendingDeletePath, ec);
            if (ec) {
                engine::core::Logger::Error("Xoa that bai: " + ec.message());
            }
            m_thumbnailCache.erase(s_pendingDeletePath);
            s_showDeleteConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Huy", ImVec2(120, 0))) {
            s_showDeleteConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void AssetBrowserPanel::Draw(EditorState& state) {
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
        else if (entry.path().extension() != ".kimeta") files.push_back(entry);
    }

    for (auto& entry : folders) DrawEntry(entry.path(), true, state);
    if (!folders.empty()) ImGui::Separator();
    for (auto& entry : files) DrawEntry(entry.path(), false, state);

    // Vung trong: chuot phai de tao moi
    ImGui::Dummy(ImGui::GetContentRegionAvail());
    if (ImGui::BeginPopupContextWindow("create_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("New Folder")) {
            std::filesystem::path newDir = m_currentDir / "New Folder";
            int suffix = 1;
            while (std::filesystem::exists(newDir)) {
                newDir = m_currentDir / ("New Folder " + std::to_string(suffix++));
            }
            std::filesystem::create_directory(newDir);
            // Mo rename ngay lap tuc, giong hanh vi Windows Explorer
            s_renamingPath = newDir.string();
            s_focusRenameInput = true;
            std::snprintf(s_renameBuffer, sizeof(s_renameBuffer), "%s", newDir.filename().string().c_str());
        }
        if (ImGui::MenuItem("New Lua Script")) {
            std::filesystem::path newFile = m_currentDir / "NewScript.lua";
            int suffix = 1;
            while (std::filesystem::exists(newFile)) {
                newFile = m_currentDir / ("NewScript" + std::to_string(suffix++) + ".lua");
            }
            std::ofstream out(newFile);
            out << "function onStart()\n    log(\"Script started for \" .. self.name)\nend\n\n"
                   "function onUpdate(dt)\n    -- your code here\nend\n";
        }
        if (ImGui::MenuItem("Show Folder in Explorer")) {
            OpenFolderInExplorer(m_currentDir);
        }
        ImGui::EndPopup();
    }

    DrawDeleteConfirmPopup();

    ImGui::End();
}

} // namespace editor