#include "engine/renderer/sprite_sheet_meta.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace engine::renderer {

static std::filesystem::path MetaPath(const std::filesystem::path& texturePath) {
    return texturePath.string() + ".kimeta";
}

SpriteSheetMeta SpriteSheetMeta::Load(const std::filesystem::path& texturePath) {
    SpriteSheetMeta meta;
    auto path = MetaPath(texturePath);
    if (!std::filesystem::exists(path)) return meta;

    std::ifstream in(path);
    nlohmann::json j;
    try {
        in >> j;
        meta.columns = j.value("columns", 1);
        meta.rows = j.value("rows", 1);
    } catch (...) {
        // file loi/hong -> tra ve mac dinh 1x1, khong crash
    }
    return meta;
}

void SpriteSheetMeta::Save(const std::filesystem::path& texturePath, const SpriteSheetMeta& meta) {
    nlohmann::json j;
    j["columns"] = meta.columns;
    j["rows"] = meta.rows;

    std::ofstream out(MetaPath(texturePath));
    out << j.dump(4);
}

} // namespace engine::renderer