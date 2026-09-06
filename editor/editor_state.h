#pragma once
#include "engine/core/game_object.h"
#include <filesystem>

class EditorState {
public:
    engine::core::GameObject* selectedObject = nullptr;
    std::filesystem::path selectedAssetPath;

    int selectedTileIndex = -1;

    void SelectGameObject(engine::core::GameObject* obj) {
        selectedObject = obj;
        selectedAssetPath.clear();
    }

    void SelectAsset(const std::filesystem::path& path) {
        selectedAssetPath = path;
        selectedObject = nullptr;
    }
};