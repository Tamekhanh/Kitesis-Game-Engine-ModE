#include "engine/core/project.h"
#include "engine/core/logger.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace engine::core
{

    std::filesystem::path Project::s_rootDir;
    std::string Project::s_name = "";
    bool Project::s_loaded = false;

    bool Project::IsPathInsideAssets(const std::filesystem::path &path)
    {
        if (!s_loaded)
            return false;
        auto assetsRoot = std::filesystem::weakly_canonical(s_rootDir / "Assets");
        auto target = std::filesystem::weakly_canonical(path);

        auto rel = std::filesystem::relative(target, assetsRoot);
        if (rel.empty())
            return false;

        std::string relStr = rel.generic_string();
        return relStr.substr(0, 2) != "..";
    }

    bool Project::CreateNew(const std::filesystem::path &rootDir, const std::string &projectName)
    {
        std::filesystem::path projectDir = rootDir / projectName;

        if (std::filesystem::exists(projectDir))
        {
            Logger::Error("Thu muc project da ton tai: " + projectDir.string());
            return false;
        }

        std::filesystem::create_directories(projectDir / "Assets");
        std::filesystem::create_directories(projectDir / "Scenes");

        nlohmann::json config;
        config["projectName"] = projectName;
        config["engineVersion"] = "0.1.0";
        config["startupScene"] = "Scenes/MainScene.kiscene";

        std::filesystem::path configFile = projectDir / (projectName + ".kiproj");
        std::ofstream out(configFile);
        if (!out.is_open())
        {
            Logger::Error("Khong the tao file project: " + configFile.string());
            return false;
        }
        out << config.dump(4); // dump(4) = in đẹp, thụt lề 4 space
        out.close();

        Logger::Info("Da tao project moi tai: " + projectDir.string());

        return Load(configFile);
    }

    bool Project::Load(const std::filesystem::path &projectFile)
    {
        if (!std::filesystem::exists(projectFile))
        {
            Logger::Error("Khong tim thay file project: " + projectFile.string());
            return false;
        }

        std::ifstream in(projectFile);
        nlohmann::json config;
        try
        {
            in >> config;
        }
        catch (const std::exception &e)
        {
            Logger::Error(std::string("Loi doc file project: ") + e.what());
            return false;
        }

        s_rootDir = projectFile.parent_path();
        s_name = config.value("projectName", "Untitled");
        s_loaded = true;

        Logger::Info("Da mo project: " + s_name + " tai " + s_rootDir.string());
        return true;
    }

    std::filesystem::path Project::AssetPath(const std::string &relativePath)
    {
        if (!s_loaded)
        {
            Logger::Error("Chua co project nao duoc mo, khong the tinh AssetPath");
            return relativePath;
        }
        return s_rootDir / "Assets" / relativePath;
    }

    std::filesystem::path Project::ScenePath(const std::string &relativePath)
    {
        if (!s_loaded)
        {
            Logger::Error("Chua co project nao duoc mo, khong the tinh ScenePath");
            return relativePath;
        }
        return s_rootDir / "Scenes" / relativePath;
    }

} // namespace engine::core