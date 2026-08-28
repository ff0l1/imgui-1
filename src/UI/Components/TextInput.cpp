#include "TextInput.h"
#include "Layout.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace UI::Components
{
    TextInput::TextInput(std::string id, std::string placeholder, bool isPassword)
        : m_Id(std::move(id))
        , m_Placeholder(std::move(placeholder))
        , m_IsPassword(isPassword)
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
        , m_FocusT(0.0f, Theme::Get().Animation().FocusSeconds)
    {
    }

    float TextInput::Height()
    {
        return Theme::Get().Metrics().InputHeight;
    }

    void TextInput::Update(float deltaSeconds)
    {
        m_HoverT.Update(deltaSeconds);
        m_FocusT.Update(deltaSeconds);
    }

    void TextInput::Draw(ImVec2 origin, float width, char* buffer, size_t bufferSize)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeMetrics& metrics = Theme::Get().Metrics();
        const ThemeTypography& typography = Theme::Get().Typography();

        const float height = metrics.InputHeight;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(drawList, 2);
        splitter.SetCurrentChannel(drawList, 1);

        ImGui::PushID(m_Id.c_str());
        ImGui::SetCursorScreenPos(origin);
        ImGui::PushFont(typography.Body);

        const float paddingY = (height - ImGui::GetFontSize()) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(metrics.InputPaddingX, paddingY));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ApplyOpacity(colors.TextPrimary));
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, ApplyOpacity(colors.TextPlaceholder));

        ImGui::SetNextItemWidth(width);
        const ImGuiInputTextFlags flags = m_IsPassword ? ImGuiInputTextFlags_Password : ImGuiInputTextFlags_None;
        ImGui::InputTextWithHint("##field", m_Placeholder.c_str(), buffer, bufferSize, flags);

        const bool focused = ImGui::IsItemActive();
        const bool hovered = ImGui::IsItemHovered();

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
        ImGui::PopFont();
        ImGui::PopID();

        m_HoverT.SetTarget(hovered ? 1.0f : 0.0f);
        m_FocusT.SetTarget(focused ? 1.0f : 0.0f);

        splitter.SetCurrentChannel(drawList, 0);

        ImVec4 surface = Mix(colors.InputSurface, colors.InputSurfaceHover, m_HoverT.Value());
        surface = Mix(surface, colors.InputSurfaceFocus, m_FocusT.Value());
        const ImVec4 border = Mix(colors.Border, colors.BorderFocus, m_FocusT.Value());

        const ImVec2 max(origin.x + width, origin.y + height);
        drawList->AddRectFilled(origin, max, ImGui::ColorConvertFloat4ToU32(ApplyOpacity(surface)), metrics.InputRadius);
        drawList->AddRect(origin, max, ImGui::ColorConvertFloat4ToU32(ApplyOpacity(border)), metrics.InputRadius, 0, 1.0f);

        splitter.Merge(drawList);
    }
}
