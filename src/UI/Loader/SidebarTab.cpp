#include "SidebarTab.h"
#include "UI/Theme/Theme.h"
#include "UI/Components/Layout.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include <algorithm>

namespace UI::Loader
{
    namespace
    {
        constexpr float kTabSize = 38.0f;
        constexpr float kTabRadius = 11.0f;
    }

    SidebarTab::SidebarTab(std::string id, std::string glyph)
        : m_Id(std::move(id))
        , m_Glyph(std::move(glyph))
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
        , m_ActiveT(0.0f, Theme::Get().Animation().FocusSeconds)
    {
    }

    float SidebarTab::Size()
    {
        return kTabSize;
    }

    void SidebarTab::Update(float deltaSeconds)
    {
        m_HoverT.Update(deltaSeconds);
        m_ActiveT.Update(deltaSeconds);
    }

    bool SidebarTab::Draw(ImVec2 origin, bool active)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        ImGui::PushID(m_Id.c_str());
        ImGui::SetCursorScreenPos(origin);
        const bool clicked = ImGui::InvisibleButton("##tab", ImVec2(kTabSize, kTabSize));
        const bool hovered = ImGui::IsItemHovered();
        ImGui::PopID();

        m_HoverT.SetTarget(hovered ? 1.0f : 0.0f);
        m_ActiveT.SetTarget(active ? 1.0f : 0.0f);

        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        const float highlight = std::max(m_ActiveT.Value(), m_HoverT.Value() * 0.55f);

        ImVec4 surface = colors.SidebarActive;
        surface.w *= highlight;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 max(origin.x + kTabSize, origin.y + kTabSize);
        drawList->AddRectFilled(origin, max,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(surface)), kTabRadius);

        const ImVec4 iconColor = Components::Mix(colors.TextSecondary, colors.TextPrimary,
            std::max(m_ActiveT.Value(), m_HoverT.Value()));

        const ImVec2 center(origin.x + kTabSize * 0.5f, origin.y + kTabSize * 0.5f);
        Components::DrawGlyphCentered(center, m_Glyph.c_str(), typography.IconSymbol, iconColor);

        return clicked;
    }
}
