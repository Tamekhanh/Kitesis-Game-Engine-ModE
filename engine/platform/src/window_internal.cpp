#include "engine/platform/window_internal.h"
#include "engine/platform/window.h"

namespace engine::platform
{

    GLFWwindow *GetGlfwHandle(Window &window)
    {
        return window.m_handle;
    }

} // namespace engine::platform