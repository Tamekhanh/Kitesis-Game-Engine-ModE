#pragma once

#include "engine/platform/window_internal.h"

struct GLFWwindow;

namespace engine::platform {

class Window;

// Chỉ dùng nội bộ trong module platform (input.cpp, ...),
// KHÔNG phải public API cho sandbox/renderer sử dụng.
GLFWwindow* GetGlfwHandle(Window& window);

} // namespace engine::platform