#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace engine::renderer { class Texture2D; }

namespace editor {

class AssetBrowserPanel {
public:
    AssetBrowserPanel();
    ~AssetBrowserPanel();

    void SetRootDirectory(const std::filesystem::path& root);
    void Draw();

private:
    engine::renderer::Texture2D* GetOrLoadThumbnail(const std::filesystem::path& path);
    void DrawImageItem(const std::filesystem::path& path, float itemWidth, float thumbSize);
    void DrawExpandedFrames(const std::filesystem::path& path, int columns, int rows, float thumbSize);

    std::filesystem::path m_rootDir;
    std::filesystem::path m_currentDir;
    std::unordered_map<std::string, std::unique_ptr<engine::renderer::Texture2D>> m_thumbnailCache;
    std::unordered_set<std::string> m_expandedItems;
};

} // namespace editor