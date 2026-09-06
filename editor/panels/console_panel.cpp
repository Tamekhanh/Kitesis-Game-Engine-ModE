#include "console_panel.h"
#include "engine/core/logger.h"
#include <imgui.h>
#include <cstring>

namespace editor {

void DrawConsolePanel() {
    ImGui::Begin("Console");

    static char filterBuffer[128] = "";
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        engine::core::Logger::ClearHistory();
    }

    ImGui::Separator();

    ImGui::BeginChild("console_scroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    auto history = engine::core::Logger::GetHistory();
    bool hasFilter = std::strlen(filterBuffer) > 0;

    for (auto& entry : history) {
        if (hasFilter && entry.message.find(filterBuffer) == std::string::npos) {
            continue;
        }

        ImVec4 color = (entry.level == engine::core::LogLevel::Error)
            ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
            : ImVec4(0.85f, 0.85f, 0.85f, 1.0f);

        const char* prefix = (entry.level == engine::core::LogLevel::Error) ? "[ERROR] " : "[INFO] ";

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted((prefix + entry.message).c_str());
        ImGui::PopStyleColor();
    }

    // Tu dong cuon xuong dong moi nhat, chi khi da o gan cuoi (khong ngat neu nguoi dung dang cuon xem lai)
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}

} // namespace editor