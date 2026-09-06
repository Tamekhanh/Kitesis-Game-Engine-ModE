#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "editor_state.h"

namespace engine::renderer { class Texture2D; }

namespace editor {

class AssetBrowserPanel {
public:
    AssetBrowserPanel();
    ~AssetBrowserPanel();

    void SetRootDirectory(const std::filesystem::path& root);
    void Draw(EditorState& state);

private:
    engine::renderer::Texture2D* GetOrLoadThumbnail(const std::filesystem::path& path);
    void DrawImageItem(const std::filesystem::path& path, EditorState& state);
    void DrawExpandedFrames(const std::filesystem::path& path, int columns, int rows, float thumbSize);
    void DrawEntry(const std::filesystem::path& path, bool isDir, EditorState& state);
    void DrawDeleteConfirmPopup();

    std::filesystem::path m_rootDir;
    std::filesystem::path m_currentDir;
    std::unordered_map<std::string, std::unique_ptr<engine::renderer::Texture2D>> m_thumbnailCache;
    std::unordered_set<std::string> m_expandedItems;
};

} // namespace editor