#include "engine/platform/window.h"
#include "engine/platform/window_internal.h"
#include "engine/core/logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

int main() {
    engine::core::Logger::Info("Starting Editor...");

    engine::platform::Window window(1280, 720, "MyEngine Editor");

    // --- Setup ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // BẮT BUỘC để bật dockspace
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    GLFWwindow* glfwHandle = engine::platform::GetGlfwHandle(window);
    ImGui_ImplGlfw_InitForOpenGL(glfwHandle, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!window.ShouldClose()) {
        window.PollEvents();

        // --- Bắt đầu frame ImGui ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Dockspace chiếm toàn màn hình ---
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // --- Panel demo tạm thời, sẽ thay bằng Hierarchy/Inspector thật ---
        ImGui::Begin("Hierarchy");
        ImGui::Text("Se hien danh sach GameObject o day");
        ImGui::End();

        ImGui::Begin("Inspector");
        ImGui::Text("Se hien Transform/Component o day");
        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::Text("Se hien game render o day");
        ImGui::End();

        // --- Render ---
        window.Clear(0.15f, 0.15f, 0.18f, 1.0f);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.SwapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}