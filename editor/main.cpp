#include "engine/platform/window.h"
#include "engine/platform/window_internal.h"
#include "engine/core/logger.h"
#include "engine/core/project.h"
#include "engine/renderer/framebuffer.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/camera2d.h"

#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <cstring>
#include <memory>

static void DrawWelcomeScreen()
{
    static char projectName[128] = "MyFirstGame";
    static char parentDir[256] = "D:/GameProjects";

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("Welcome", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.3f);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 200.0f);

    ImGui::BeginChild("CenterBox", ImVec2(400, 220), true);

    ImGui::Text("Tao Project Moi");
    ImGui::Separator();

    ImGui::Text("Ten project:");
    ImGui::InputText("##name", projectName, sizeof(projectName));

    ImGui::Text("Thu muc chua project:");
    ImGui::InputText("##dir", parentDir, sizeof(parentDir));

    ImGui::Spacing();

    if (ImGui::Button("Tao Project", ImVec2(-1, 0)))
    {
        bool ok = engine::core::Project::CreateNew(parentDir, projectName);
        if (!ok)
        {
            engine::core::Logger::Error("Tao project that bai, kiem tra log phia tren");
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

int main()
{
    engine::core::Logger::Info("Starting Editor...");

    engine::platform::Window window(1280, 720, "MyEngine Editor");
    GLFWwindow *glfwHandle = engine::platform::GetGlfwHandle(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(glfwHandle, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    auto framebuffer = std::make_unique<engine::renderer::Framebuffer>(1280, 720);
    engine::renderer::SpriteRenderer sceneRenderer;
    engine::renderer::Camera2D sceneCamera(1280.0f, 720.0f);
    while (!window.ShouldClose())
    {
        window.PollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!engine::core::Project::IsLoaded())
        {
            DrawWelcomeScreen();
        }
        else
        {
            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

            ImGui::Begin("Hierarchy");
            ImGui::Text("Project: %s", engine::core::Project::Name().c_str());
            ImGui::End();

            ImGui::Begin("Inspector");
            ImGui::Text("Se hien Transform/Component o day");
            ImGui::End();

            ImGui::Begin("Scene");

            ImVec2 panelSize = ImGui::GetContentRegionAvail();
            if (panelSize.x > 0 && panelSize.y > 0)
            {
                framebuffer->Resize((int)panelSize.x, (int)panelSize.y);
                sceneCamera.Resize(panelSize.x, panelSize.y);
            }

            // --- Vẽ game vào framebuffer (không phải màn hình thật) ---
            framebuffer->Bind();
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            sceneRenderer.Begin(sceneCamera);
            sceneRenderer.DrawQuad(50.0f, 50.0f, 100.0f, 100.0f, 1.0f, 0.3f, 0.3f);

            framebuffer->Unbind();

            // --- Hiện texture kết quả vào panel ImGui ---
            ImGui::Image(
                (ImTextureID)(intptr_t)framebuffer->GetColorTextureId(),
                panelSize,
                ImVec2(0, 1), ImVec2(1, 0) // lật UV theo trục Y
            );

            ImGui::End();
        }

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