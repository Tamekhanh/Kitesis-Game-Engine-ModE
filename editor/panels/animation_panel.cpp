#include "animation_panel.h"
#include "engine/renderer/animation_component.h"
#include <imgui.h>
#include <filesystem>

namespace editor {

void DrawAnimationPanel(EditorState& state) {
    ImGui::Begin("Animation");

    if (!state.selectedObject) {
        ImGui::TextDisabled("Chua chon GameObject nao");
        ImGui::End();
        return;
    }

    auto* anim = state.selectedObject->GetComponent<engine::renderer::AnimationComponent>();
    if (!anim) {
        ImGui::TextDisabled("GameObject nay chua co AnimationComponent");
        ImGui::TextDisabled("(Add Component o Inspector de them)");
        ImGui::End();
        return;
    }

    // --- Thanh dieu khien phat ---
    if (anim->IsPlaying()) {
        if (ImGui::Button("Stop")) anim->Stop();
    } else {
        if (ImGui::Button("Play")) anim->Play();
    }
    ImGui::SameLine();

    float duration = anim->FrameDuration();
    ImGui::SetNextItemWidth(120);
    if (ImGui::DragFloat("Frame Duration (s)", &duration, 0.01f, 0.01f, 2.0f)) {
        anim->SetFrameDuration(duration);
    }
    ImGui::SameLine();

    bool looping = anim->Looping();
    if (ImGui::Checkbox("Looping", &looping)) {
        anim->SetLooping(looping);
    }

    ImGui::Separator();

    // --- Vung tha anh/frame vao de them ---
    ImGui::Button("Keo anh hoac frame tu Assets vao day de them", ImVec2(-1, 40));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_PATH")) {
            std::string path(static_cast<const char*>(payload->Data));
            anim->AddFrame(path, 0);
        }
        else if (const ImGuiPayload* framePayload = ImGui::AcceptDragDropPayload("ASSET_IMAGE_FRAME")) {
            std::string encoded(static_cast<const char*>(framePayload->Data));
            size_t sep = encoded.rfind('|');
            if (sep != std::string::npos) {
                std::string path = encoded.substr(0, sep);
                int frameIndex = std::stoi(encoded.substr(sep + 1));
                anim->AddFrame(path, frameIndex);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Separator();
    ImGui::Text("Danh sach frame (thu tu chay tu tren xuong):");
    ImGui::Spacing();

    // --- Danh sach frame dang co, dang "dope sheet" doc ---
    int removeIndex = -1;
    int moveUpIndex = -1;
    int moveDownIndex = -1;

    auto& frames = anim->Frames();
    for (int i = 0; i < (int)frames.size(); ++i) {
        ImGui::PushID(i);

        bool isCurrent = (anim->IsPlaying() && i == anim->CurrentIndex());
        if (isCurrent) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.5f, 0.9f, 0.3f));
        }

        ImGui::BeginChild("row", ImVec2(-1, 50), true);

        unsigned int texId = anim->GetOrLoadThumbnailId(frames[i].texturePath);
        if (texId != 0) {
            ImGui::Image((ImTextureID)(intptr_t)texId, ImVec2(36, 36), ImVec2(0, 1), ImVec2(1, 0));
        } else {
            ImGui::Dummy(ImVec2(36, 36));
        }
        ImGui::SameLine();

        std::filesystem::path p(frames[i].texturePath);
        ImGui::BeginGroup();
        ImGui::Text("%d: %s", i, p.filename().string().c_str());
        ImGui::Text("Frame trong anh: %d", frames[i].frameIndex);
        ImGui::EndGroup();

        ImGui::SameLine(ImGui::GetWindowWidth() - 110);
        if (ImGui::SmallButton("Up")) moveUpIndex = i;
        ImGui::SameLine();
        if (ImGui::SmallButton("Down")) moveDownIndex = i;
        ImGui::SameLine();
        if (ImGui::SmallButton("X")) removeIndex = i;

        ImGui::EndChild();

        if (isCurrent) {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
    }

    if (removeIndex >= 0) anim->RemoveFrame(removeIndex);
    if (moveUpIndex >= 0) anim->MoveFrame(moveUpIndex, -1);
    if (moveDownIndex >= 0) anim->MoveFrame(moveDownIndex, 1);

    if (!frames.empty() && ImGui::Button("Xoa tat ca")) {
        anim->ClearFrames();
    }

    ImGui::End();
}

} // namespace editor