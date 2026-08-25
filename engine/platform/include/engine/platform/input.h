#pragma once

namespace engine::platform {

class Window;

enum class Key {
    W, A, S, D,
    Up, Down, Left, Right,
    Space, Escape,
};

class Input {
public:
    static void Init(Window& window);
    static bool IsKeyDown(Key key);

    static float MouseX();
    static float MouseY();
    static bool IsMouseButtonDown(int button);

    // Gọi cuối mỗi frame để cập nhật trạng thái "vừa nhấn/vừa thả"
    static void EndFrame();
};

} // namespace engine::platform