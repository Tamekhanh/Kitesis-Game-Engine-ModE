#pragma once
#include <filesystem>

namespace engine::renderer {

struct SpriteSheetMeta {
    int columns = 1;
    int rows = 1;

    static SpriteSheetMeta Load(const std::filesystem::path& texturePath);
    static void Save(const std::filesystem::path& texturePath, const SpriteSheetMeta& meta);
};

} // namespace engine::renderer