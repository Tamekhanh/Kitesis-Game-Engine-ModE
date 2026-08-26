#pragma once
#include <filesystem>
#include <string>

namespace editor {

class AssetBrowserPanel {
public:
    AssetBrowserPanel();

    void SetRootDirectory(const std::filesystem::path& root);
    void Draw();

private:
    std::filesystem::path m_rootDir;
    std::filesystem::path m_currentDir;
};

} // namespace editor