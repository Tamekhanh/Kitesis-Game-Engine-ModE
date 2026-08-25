#pragma once
#include "engine/math/vector2.h"

struct GLFWwindow;

namespace engine::platform {

GLFWwindow* GetGlfwHandle(class Window& window); // forward declare friend

class Window {
public:
    static float GetTime();
    Window(int width, int height, const char* title);
    ~Window();

    bool ShouldClose() const;
    void PollEvents();
    void SwapBuffers();
    void Clear(float r, float g, float b, float a = 1.0f);

private:
    friend GLFWwindow* GetGlfwHandle(Window& window);
    GLFWwindow* m_handle = nullptr;
};

} // namespace engine::platform