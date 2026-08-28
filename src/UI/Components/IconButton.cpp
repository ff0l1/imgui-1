#include "IconButton.h"
#include "Layout.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"

namespace UI::Components
{
    IconButton::IconButton(std::string id, std::string glyph, ImFont* font, const IconButtonStyle& style)
        : m_Id(std::move(id))
        , m_Glyph(std::move(glyph))
        , m_Font(font)
        , m_Style(style)
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
    {
    }

    void IconButton::Update(float deltaSeconds)
    {
        m_HoverT.Update(deltaSeconds);
    }

    bool IconButton::Draw(ImVec2 origin)
    {
        const float size = m_Style.Size;

        ImGui::PushID(m_Id.c_str());
        ImGui::SetCursorScreenPos(origin);
        const bool clicked = ImGui::InvisibleButton("##icon", ImVec2(size, size));
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();
        ImGui::PopID();

        m_HoverT.SetTarget(hovered ? 1.0f : 0.0f);
        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 max(origin.x + size, origin.y + size);
        const ImVec2 center(origin.x + size * 0.5f, origin.y + size * 0.5f);

        const float hover = m_HoverT.Value();

        if (m_Style.FilledSurface || hover > 0.001f)
        {
            ImVec4 surface = m_Style.HoverSurface;
            surface.w *= hover;
            const ImU32 surfaceColor = ImGui::ColorConvertFloat4ToU32(ApplyOpacity(surface));
            if (m_Style.Circular)
                drawList->AddCircleFilled(center, size * 0.5f, surfaceColor, 48);
            else
                drawList->AddRectFilled(origin, max, surfaceColor, 8.0f);
        }

        ImVec4 iconColor = Mix(m_Style.IdleColor, m_Style.HoverColor, hover);
        if (held && m_Style.PressedColor.w > 0.0f)
            iconColor = m_Style.PressedColor;
        DrawGlyphCentered(center, m_Glyph.c_str(), m_Font, iconColor);

        return clicked;
    }
}
