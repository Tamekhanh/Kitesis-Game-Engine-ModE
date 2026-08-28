#include "engine/platform/input.h"
#include "engine/platform/window_internal.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace engine::platform
{

    static GLFWwindow *s_handle = nullptr;

    static int ToGlfwKey(Key key)
    {
        switch (key)
        {
        case Key::W:
            return GLFW_KEY_W;
        case Key::A:
            return GLFW_KEY_A;
        case Key::S:
            return GLFW_KEY_S;
        case Key::D:
            return GLFW_KEY_D;
        case Key::Up:
            return GLFW_KEY_UP;
        case Key::Down:
            return GLFW_KEY_DOWN;
        case Key::Left:
            return GLFW_KEY_LEFT;
        case Key::Right:
            return GLFW_KEY_RIGHT;
        case Key::Space:
            return GLFW_KEY_SPACE;
        case Key::Escape:
            return GLFW_KEY_ESCAPE;
        }
        return GLFW_KEY_UNKNOWN;
    }

    void Input::Init(Window &window)
    {
        s_handle = GetGlfwHandle(window);
    }

    bool Input::IsKeyDown(Key key)
    {
        if (!s_handle)
            return false;
        return glfwGetKey(s_handle, ToGlfwKey(key)) == GLFW_PRESS;
    }

    float Input::MouseX()
    {
        double x = 0, y = 0;
        glfwGetCursorPos(s_handle, &x, &y);
        return static_cast<float>(x);
    }

    float Input::MouseY()
    {
        double x = 0, y = 0;
        glfwGetCursorPos(s_handle, &x, &y);
        return static_cast<float>(y);
    }

    bool Input::IsMouseButtonDown(int button)
    {
        if (!s_handle)
            return false;
        return glfwGetMouseButton(s_handle, button) == GLFW_PRESS;
    }

    void Input::EndFrame()
    {
        // Chỗ này để trống hiện tại, dùng sau khi thêm "just pressed" detection
    }

} // namespace engine::platform