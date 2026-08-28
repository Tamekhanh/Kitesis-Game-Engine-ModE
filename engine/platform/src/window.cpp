#include "engine/platform/window.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>

namespace engine::platform
{

    Window::Window(int width, int height, const char *title)
    {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwMakeContextCurrent(m_handle);

        int version = gladLoaderLoadGL();
        if (version == 0)
        {
            std::printf("[ERROR] Failed to load OpenGL via glad\n");
        }
        else
        {
            std::printf("[INFO] Loaded OpenGL %d.%d\n",
                        GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
        }
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    float Window::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }

    bool Window::ShouldClose() const { return glfwWindowShouldClose(m_handle); }
    void Window::PollEvents() { glfwPollEvents(); }
    void Window::SwapBuffers() { glfwSwapBuffers(m_handle); }

    void Window::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

} // namespace engine::platform
