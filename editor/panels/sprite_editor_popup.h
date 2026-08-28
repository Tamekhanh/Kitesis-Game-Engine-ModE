#pragma once
#include <filesystem>

namespace engine::renderer
{
    class SpriteComponent;
}

namespace editor
{

    // Mo Sprite Editor cho 1 duong dan anh bat ky (vd: goi tu Asset Browser)
    void OpenSpriteEditorForPath(const std::filesystem::path &texturePath);

    // Mo Sprite Editor va lien ket voi 1 SpriteComponent cu the (goi tu Inspector),
    // de sau khi Apply, component do duoc cap nhat ngay lap tuc
    void OpenSpriteEditorForComponent(engine::renderer::SpriteComponent &sprite);

    void DrawSpriteEditorPopup();

}