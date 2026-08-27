#include "engine/platform/window.h"
#include "engine/platform/window_internal.h"
#include "engine/core/logger.h"
#include "engine/core/project.h"
#include "engine/renderer/framebuffer.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/camera2d.h"
#include "engine/renderer/texture2d.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/scene_serializer.h"
#include "engine/renderer/sprite_component.h"

#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <cstring>
#include <memory>

#include "editor_state.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/asset_browser_panel.h"
#include "panels/sprite_editor_popup.h"
#include "panels/animation_panel.h"
#include <vector>
#include "engine/core/game_object.h"

static void DrawWelcomeScreen()
{
    static char projectName[128] = "MyFirstGame";
    static char parentDir[256] = "D:/GameProjects";
    static char openProjectPath[256] = "D:/GameProjects/MyFirstGame/MyFirstGame.kiproj";
    static std::string errorMessage;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("Welcome", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.2f);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 220.0f);

    ImGui::BeginChild("CenterBox", ImVec2(440, 480), true);

    // ---------------- Tạo Project mới ----------------
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
            errorMessage = "Tao project that bai, kiem tra console log";
        }
        else
        {
            errorMessage.clear();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // ---------------- Mở Project đã có ----------------
    ImGui::Text("Mo Project Da Co");
    ImGui::Separator();

    ImGui::Text("Duong dan file .kiproj:");
    ImGui::InputText("##openpath", openProjectPath, sizeof(openProjectPath));

    ImGui::Spacing();

    if (ImGui::Button("Mo Project", ImVec2(-1, 0)))
    {
        bool ok = engine::core::Project::Load(openProjectPath);
        if (!ok)
        {
            errorMessage = "Mo project that bai, kiem tra duong dan file .kiproj";
        }
        else
        {
            errorMessage.clear();
        }
    }

    // ---------------- Thông báo lỗi (nếu có) ----------------
    if (!errorMessage.empty())
    {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", errorMessage.c_str());
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
    engine::renderer::SpriteComponent::SetDefaultRenderer(&sceneRenderer);

    engine::renderer::Camera2D sceneCamera(1280.0f, 720.0f);

    EditorState editorState;
    editor::AssetBrowserPanel assetBrowser;
    std::vector<std::unique_ptr<engine::renderer::Texture2D>> textureLibrary;
    engine::core::GameObject sceneRoot("SceneRoot");
    std::unique_ptr<engine::renderer::Texture2D> demoTexture;
    bool sceneBuilt = false;
    float lastTime = engine::platform::Window::GetTime();

    while (!window.ShouldClose())
    {
        window.PollEvents();

        float currentTime = engine::platform::Window::GetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!engine::core::Project::IsLoaded())
        {
            DrawWelcomeScreen();
        }
        else
        {
            // --- Xây scene demo đúng 1 lần, ngay khi project vừa mở xong ---
            if (!sceneBuilt)
            {
                std::string spritePath = engine::core::Project::AssetPath("sprite.png").string();
                demoTexture = std::make_unique<engine::renderer::Texture2D>(spritePath);

                auto enemyOwned = std::make_unique<engine::core::GameObject>("Enemy");
                engine::core::GameObject *enemy = sceneRoot.AddChild(std::move(enemyOwned));
                enemy->transform.position = {200.0f, 150.0f};
                auto *enemySprite = enemy->AddComponent<engine::renderer::SpriteComponent>(sceneRenderer, 80.0f, 80.0f);
                enemySprite->BindTexture(*demoTexture, spritePath);

                auto playerOwned = std::make_unique<engine::core::GameObject>("Player");
                engine::core::GameObject *player = sceneRoot.AddChild(std::move(playerOwned));
                player->transform.position = {50.0f, 50.0f};
                auto *playerSprite = player->AddComponent<engine::renderer::SpriteComponent>(sceneRenderer, 60.0f, 60.0f);
                playerSprite->BindTexture(*demoTexture, spritePath);

                assetBrowser.SetRootDirectory(engine::core::Project::AssetPath(""));

                sceneBuilt = true;

                sceneBuilt = true;
            }

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save Scene"))
                    {
                        engine::renderer::SceneSerializer::Save(
                            sceneRoot, engine::core::Project::ScenePath("MainScene.kiscene"));
                    }
                    if (ImGui::MenuItem("Open Scene"))
                    {
                        engine::renderer::SceneSerializer::Load(
                            sceneRoot, engine::core::Project::ScenePath("MainScene.kiscene"),
                            sceneRenderer, textureLibrary);
                        editorState.selectedObject = nullptr;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

            editor::DrawHierarchyPanel(sceneRoot, editorState);
            editor::DrawInspectorPanel(editorState);
            editor::DrawAnimationPanel(editorState);
            assetBrowser.Draw();
            editor::DrawSpriteEditorPopup();

            ImGui::Begin("Scene");

            ImVec2 panelSize = ImGui::GetContentRegionAvail();
            if (panelSize.x > 0 && panelSize.y > 0)
            {
                framebuffer->Resize((int)panelSize.x, (int)panelSize.y);
                sceneCamera.Resize(panelSize.x, panelSize.y);
            }

            framebuffer->Bind();
            glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            sceneRoot.Update(deltaTime);

            sceneRenderer.Begin(sceneCamera);
            sceneRoot.Render();

            framebuffer->Unbind();

            ImGui::Image(
                (ImTextureID)(intptr_t)framebuffer->GetColorTextureId(),
                panelSize,
                ImVec2(0, 1), ImVec2(1, 0));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_PATH"))
                {
                    std::string droppedPath(static_cast<const char *>(payload->Data));

                    textureLibrary.push_back(
                        std::make_unique<engine::renderer::Texture2D>(droppedPath));

                    std::filesystem::path p(droppedPath);
                    auto newObjOwned = std::make_unique<engine::core::GameObject>(p.stem().string());
                    engine::core::GameObject *newObj = sceneRoot.AddChild(std::move(newObjOwned));
                    newObj->transform.position = {100.0f, 100.0f};

                    auto *sprite = newObj->AddComponent<engine::renderer::SpriteComponent>(
                        sceneRenderer, 80.0f, 80.0f);
                    sprite->BindTexture(*textureLibrary.back(), droppedPath);

                    editorState.selectedObject = newObj;
                }
                ImGui::EndDragDropTarget();
            }

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