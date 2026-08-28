#pragma once
#include "engine/core/game_object.h"

class EditorState {
public:
    engine::core::GameObject* selectedObject = nullptr;

    int selectedTileIndex = -1;
};