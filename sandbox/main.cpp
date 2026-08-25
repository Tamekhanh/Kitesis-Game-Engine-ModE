#include "engine/platform/window.h"
#include "engine/platform/input.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/camera2d.h"
#include "engine/renderer/texture2d.h"
#include "engine/renderer/sprite_component.h"
#include "engine/core/game_object.h"
#include "engine/core/logger.h"
#include "move_component.h"

int main() {
    engine::core::Logger::Info("Starting engine...");

    const float kWidth = 800.0f;
    const float kHeight = 600.0f;

    engine::platform::Window window(800, 600, "My 2D Game");
    engine::platform::Input::Init(window);

    engine::renderer::SpriteRenderer spriteRenderer;
    engine::renderer::Camera2D camera(kWidth, kHeight);
    engine::renderer::Texture2D texture("assets/sprite.png");

    // --- Xây scene bằng GameObject ---
    engine::core::GameObject root("Root");

    auto playerOwned = std::make_unique<engine::core::GameObject>("Player");
    engine::core::GameObject* player = root.AddChild(std::move(playerOwned));
    player->transform.position = {350.0f, 250.0f};
    player->AddComponent<engine::renderer::SpriteComponent>(spriteRenderer, texture, 100.0f, 100.0f);
    player->AddComponent<MoveComponent>(200.0f); // 200 pixel/giây

    float lastTime = 0.0f;

    while (!window.ShouldClose()) {
        window.PollEvents();

        float currentTime = engine::platform::Window::GetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        root.Update(deltaTime);

        window.Clear(0.1f, 0.2f, 0.6f, 1.0f);
        spriteRenderer.Begin(camera);
        root.Render();
        window.SwapBuffers();
    }

    return 0;
}