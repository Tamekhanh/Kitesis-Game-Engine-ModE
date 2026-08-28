#pragma once
#include <string>
#include <filesystem>

namespace engine::core
{

    class Project
    {
    public:
        static bool CreateNew(const std::filesystem::path &rootDir, const std::string &projectName);
        static bool Load(const std::filesystem::path &projectFile);

        static std::filesystem::path AssetPath(const std::string &relativePath);
        static std::filesystem::path ScenePath(const std::string &relativePath);

        static const std::string &Name() { return s_name; }
        static const std::filesystem::path &RootDir() { return s_rootDir; }
        static bool IsLoaded() { return s_loaded; }

    private:
        static std::filesystem::path s_rootDir;
        static std::string s_name;
        static bool s_loaded;
    };

} // namespace engine::core