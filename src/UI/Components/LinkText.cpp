#include "LinkText.h"
#include "Layout.h"
#include "UI/Theme/Theme.h"

#include "imgui.h"

namespace UI::Components
{
    LinkText::LinkText(std::string text)
        : m_Text(std::move(text))
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
    {
    }

    float LinkText::Height()
    {
        return TextHeight(Theme::Get().Typography().Small);
    }

    void LinkText::Update(float deltaSeconds)
    {
        m_HoverT.Update(deltaSeconds);
    }

    bool LinkText::Draw(ImVec2 origin, float width)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        ImFont* font = Theme::Get().Typography().Small;

        const float textWidth = TextWidth(m_Text.c_str(), font);
        const float height = TextHeight(font);
        const ImVec2 textOrigin(origin.x + (width - textWidth) * 0.5f, origin.y);

        ImGui::PushID(m_Text.c_str());
        ImGui::SetCursorScreenPos(textOrigin);
        const bool clicked = ImGui::InvisibleButton("##link", ImVec2(textWidth, height));
        const bool hovered = ImGui::IsItemHovered();
        ImGui::PopID();

        m_HoverT.SetTarget(hovered ? 1.0f : 0.0f);
        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        const ImVec4 color = Mix(colors.TextSecondary, colors.TextPrimary, m_HoverT.Value());
        DrawTextAt(textOrigin, m_Text.c_str(), font, color);

        return clicked;
    }
}
