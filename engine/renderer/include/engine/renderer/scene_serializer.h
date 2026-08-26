#pragma once
#include "engine/core/game_object.h"
#include <filesystem>
#include <vector>
#include <memory>

namespace engine::renderer {

class SpriteRenderer;
class Texture2D;

class SceneSerializer {
public:
    static void Save(engine::core::GameObject& root, const std::filesystem::path& path);

    static void Load(engine::core::GameObject& root,
                      const std::filesystem::path& path,
                      SpriteRenderer& renderer,
                      std::vector<std::unique_ptr<Texture2D>>& textureLibrary);
};

} // namespace engine::renderer