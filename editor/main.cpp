#include "engine/platform/window.h"
#include "engine/platform/window_internal.h"
#include "engine/core/logger.h"
#include "engine/core/project.h"
#include "engine/core/game_object.h"
#include "engine/core/collider_component.h"
#include "engine/core/tilemap_collider_component.h"
#include "engine/core/physics_world.h"
#include "engine/core/box_collider_component.h"
#include "engine/core/circle_collider_component.h"

#include "engine/renderer/framebuffer.h"
#include "engine/renderer/sprite_renderer.h"
#include "engine/renderer/camera2d.h"
#include "engine/renderer/texture2d.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/scene_serializer.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/tilemap_component.h"
#include "engine/renderer/tilemap_collider_baker.h"

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
#include "panels/tile_palette_panel.h"
#include <vector>

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
    engine::renderer::TilemapComponent::SetDefaultRenderer(&sceneRenderer);

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

                // temp tilemap
                auto tilemapOwned = std::make_unique<engine::core::GameObject>("TestTilemap");
                engine::core::GameObject *tilemapObj = sceneRoot.AddChild(std::move(tilemapOwned));
                tilemapObj->transform.position = {400.0f, 50.0f};

                auto *tilemap = tilemapObj->AddComponent<engine::renderer::TilemapComponent>();
                tilemap->Resize(5, 5);
                tilemap->BindTileset(*demoTexture, spritePath);
                tilemap->SetTileSize(32.0f);

                // Ve mot vai o test, dung tile index 0 (o dau tien trong tileset)
                for (int x = 0; x < 5; ++x)
                {
                    tilemap->SetTile(x, 4, 0); // hang duoi cung: ve dam dac
                }
                tilemap->SetTile(2, 2, 0);
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
                        editorState.SelectGameObject(nullptr);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

            editor::DrawHierarchyPanel(sceneRoot, editorState);
            editor::DrawInspectorPanel(editorState);
            editor::DrawAnimationPanel(editorState);
            editor::DrawTilePalettePanel(editorState);
            assetBrowser.Draw(editorState);
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
            engine::core::PhysicsWorld::Step(sceneRoot, deltaTime);

            sceneRenderer.Begin(sceneCamera);
            sceneRoot.Render();

            framebuffer->Unbind();

            ImGui::Image(
                (ImTextureID)(intptr_t)framebuffer->GetColorTextureId(),
                panelSize,
                ImVec2(0, 1), ImVec2(1, 0));

            // --- Ve outline gioi han cua Tilemap, hien bat ke co dang cam tile hay khong ---
            if (editorState.selectedObject)
            {
                auto *tilemapForOutline = editorState.selectedObject->GetComponent<engine::renderer::TilemapComponent>();
                if (tilemapForOutline)
                {
                    ImVec2 imageMin = ImGui::GetItemRectMin();
                    auto worldPos = editorState.selectedObject->GetWorldPosition();
                    float tileSize = tilemapForOutline->TileSize();
                    float gridW = tilemapForOutline->Width() * tileSize;
                    float gridH = tilemapForOutline->Height() * tileSize;

                    ImDrawList *drawList = ImGui::GetWindowDrawList();
                    ImVec2 gridMin(imageMin.x + worldPos.x, imageMin.y + worldPos.y);
                    ImVec2 gridMax(gridMin.x + gridW, gridMin.y + gridH);

                    // Khung ngoai cung, ro net
                    drawList->AddRect(gridMin, gridMax, IM_COL32(0, 255, 180, 255), 0.0f, 0, 2.5f);

                    // Ke luoi mo ben trong, giup nhin ro tung o
                    for (int x = 1; x < tilemapForOutline->Width(); ++x)
                    {
                        float lx = gridMin.x + x * tileSize;
                        drawList->AddLine(ImVec2(lx, gridMin.y), ImVec2(lx, gridMax.y),
                                          IM_COL32(0, 255, 180, 60));
                    }
                    for (int y = 1; y < tilemapForOutline->Height(); ++y)
                    {
                        float ly = gridMin.y + y * tileSize;
                        drawList->AddLine(ImVec2(gridMin.x, ly), ImVec2(gridMax.x, ly),
                                          IM_COL32(0, 255, 180, 60));
                    }
                }
            }

            // --- Outline cho ColliderComponent don (co the nhieu cai tren cung GameObject) ---
            if (editorState.selectedObject)
            {
                ImVec2 imageMin = ImGui::GetItemRectMin();
                ImDrawList *drawList = ImGui::GetWindowDrawList();

                for (auto *box : editorState.selectedObject->GetComponents<engine::core::BoxColliderComponent>())
                {
                    engine::math::Vector2 corners[4];
                    box->GetCorners(corners);

                    ImU32 color = box->isTrigger
                                      ? IM_COL32(255, 100, 255, 220)
                                      : IM_COL32(255, 60, 60, 220);

                    for (int i = 0; i < 4; ++i)
                    {
                        int next = (i + 1) % 4;
                        ImVec2 p1(imageMin.x + corners[i].x, imageMin.y + corners[i].y);
                        ImVec2 p2(imageMin.x + corners[next].x, imageMin.y + corners[next].y);
                        drawList->AddLine(p1, p2, color, 2.0f);
                    }
                }

                for (auto *circle : editorState.selectedObject->GetComponents<engine::core::CircleColliderComponent>())
                {
                    auto center = circle->WorldCenter();
                    ImVec2 c(imageMin.x + center.x, imageMin.y + center.y);

                    ImU32 color = circle->isTrigger
                                      ? IM_COL32(255, 100, 255, 220)
                                      : IM_COL32(255, 60, 60, 220);

                    drawList->AddCircle(c, circle->radius, color, 32, 2.0f);
                }
            }

            // --- Outline cho TilemapColliderComponent ---
            if (editorState.selectedObject)
            {
                auto *tmCollider = editorState.selectedObject->GetComponent<engine::core::TilemapColliderComponent>();
                if (tmCollider)
                {
                    ImVec2 imageMin = ImGui::GetItemRectMin();
                    ImU32 color = tmCollider->isTrigger
                                      ? IM_COL32(255, 100, 255, 200)
                                      : IM_COL32(255, 60, 60, 200);

                    for (int i = 0; i < (int)tmCollider->rects.size(); ++i)
                    {
                        float minX, minY, maxX, maxY;
                        tmCollider->GetWorldBounds(i, minX, minY, maxX, maxY);
                        ImVec2 rectMin(imageMin.x + minX, imageMin.y + minY);
                        ImVec2 rectMax(imageMin.x + maxX, imageMin.y + maxY);
                        ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, color, 0.0f, 0, 2.0f);
                    }
                }
            }

            // --- Highlight tong quat cho GameObject dang chon (khung vang mo, luon hien) ---
            if (editorState.selectedObject)
            {
                ImVec2 imageMin = ImGui::GetItemRectMin();
                auto worldPos = editorState.selectedObject->GetWorldPosition();

                float hx = 16.0f, hy = 16.0f; // kich thuoc mac dinh neu khong co gi khac de tham chieu

                // Uu tien lay kich thuoc tu SpriteComponent neu co, cho khung khop dung sprite
                if (auto *sprite = editorState.selectedObject->GetComponent<engine::renderer::SpriteComponent>())
                {
                    hx = sprite->Width() * 0.5f;
                    hy = sprite->Height() * 0.5f;
                }

                ImVec2 center(imageMin.x + worldPos.x, imageMin.y + worldPos.y);
                ImVec2 pMin(center.x - hx, center.y - hy);
                ImVec2 pMax(center.x + hx, center.y + hy);
                ImGui::GetWindowDrawList()->AddRect(pMin, pMax, IM_COL32(255, 220, 0, 160), 0.0f, 0, 1.5f);

                // Diem tam, luon hien du co Sprite hay khong (vd GameObject rong)
                ImGui::GetWindowDrawList()->AddCircleFilled(center, 4.0f, IM_COL32(255, 220, 0, 255));
            }

            // --- Ve tile bang chuot, chi khi dang chon GameObject co TilemapComponent VA da chon tile ---
            if (editorState.selectedObject && editorState.selectedTileIndex >= 0)
            {
                auto *tilemap = editorState.selectedObject->GetComponent<engine::renderer::TilemapComponent>();
                if (tilemap && ImGui::IsItemHovered())
                {
                    ImVec2 imageMin = ImGui::GetItemRectMin();
                    ImVec2 mousePos = ImGui::GetMousePos();

                    // Toa do pixel tuong doi trong framebuffer (0,0 la goc tren-trai panel Scene)
                    float localX = mousePos.x - imageMin.x;
                    float localY = mousePos.y - imageMin.y;

                    auto worldPos = editorState.selectedObject->GetWorldPosition();
                    float tileSize = tilemap->TileSize();

                    // Doi toa do pixel panel -> toa do o luoi
                    int tileX = (int)((localX - worldPos.x) / tileSize);
                    int tileY = (int)((localY - worldPos.y) / tileSize);

                    if (tileX >= 0 && tileX < tilemap->Width() && tileY >= 0 && tileY < tilemap->Height())
                    {
                        // Highlight o dang tro toi
                        ImDrawList *drawList = ImGui::GetWindowDrawList();
                        ImVec2 cellMin(imageMin.x + worldPos.x + tileX * tileSize,
                                       imageMin.y + worldPos.y + tileY * tileSize);
                        ImVec2 cellMax(cellMin.x + tileSize, cellMin.y + tileSize);
                        drawList->AddRect(cellMin, cellMax, IM_COL32(255, 255, 0, 200), 0.0f, 0, 2.0f);

                        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        {
                            tilemap->SetTile(tileX, tileY, editorState.selectedTileIndex);
                        }
                        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                        {
                            tilemap->SetTile(tileX, tileY, -1);
                        }

                        // Tu dong cap nhat lai collider ngay khi tha chuot (ket thuc 1 net ve),
                        // chi ap dung neu GameObject nay da tung Generate Collider truoc do
                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) ||
                            ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                        {
                            auto *existingCollider =
                                editorState.selectedObject->GetComponent<engine::core::TilemapColliderComponent>();
                            if (existingCollider)
                            {
                                engine::renderer::BakeTilemapCollider(*tilemap, *existingCollider);
                            }
                        }
                    }
                }
            }

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

                    editorState.SelectGameObject(newObj);
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