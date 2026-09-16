#include "EditorWidgets.h"
#include <imgui.h>

void drawVector2(const char *label, Vec2f &value) {
    if (ImGui::BeginTable(label, 3, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("##property", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 0));
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("X");
        ImGui::TableSetColumnIndex(2);
        ImGui::TextDisabled("Y");
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##x", &value.x);
        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##y", &value.y);
        ImGui::EndTable();
    }
}

void drawVector2(const char *label, Vector2<int> &value) {
    if (ImGui::BeginTable(label, 3, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("##property", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 0));
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("X");
        ImGui::TableSetColumnIndex(2);
        ImGui::TextDisabled("Y");
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##x", &value.x);
        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##y", &value.y);
        ImGui::EndTable();
    }
}

void drawColor(const char *label, sf::Color &color) {
    float values[4] = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

    if (ImGui::ColorEdit4(label, values)) {
        color = sf::Color(static_cast<std::uint8_t>(values[0] * 255.0f), static_cast<std::uint8_t>(values[1] * 255.0f), static_cast<std::uint8_t>(values[2] * 255.0f),
                          static_cast<std::uint8_t>(values[3] * 255.0f));
    }
}
