#include "GlassPanel.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"

namespace UI::Components
{
    namespace
    {
        constexpr float kShadowExpand = 2.0f;
        constexpr float kShadowDrop = 4.0f;
        constexpr float kShadowAlpha = 0.14f;
    }

    void DrawGlassPanel(ImVec2 min, ImVec2 max, const GlassPanelStyle& style)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        if (style.Shadow)
        {
            ImVec4 shadow = colors.Shadow;
            shadow.w *= kShadowAlpha;
            drawList->AddRectFilled(
                ImVec2(min.x - kShadowExpand, min.y + kShadowDrop),
                ImVec2(max.x + kShadowExpand, max.y + kShadowExpand + kShadowDrop),
                ImGui::ColorConvertFloat4ToU32(ApplyOpacity(shadow)),
                style.Radius + kShadowExpand);
        }

        const ImVec4 surface = style.CustomSurface ? style.SurfaceColor : colors.GlassSurface;
        drawList->AddRectFilled(min, max,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(surface)), style.Radius);

        if (style.Sheen)
        {
            drawList->AddRect(
                ImVec2(min.x + 1.0f, min.y + 1.0f), ImVec2(max.x - 1.0f, max.y - 1.0f),
                ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.GlassSheen)),
                style.Radius - 1.0f, 0, 1.0f);
        }

        if (style.Border)
        {
            drawList->AddRect(min, max,
                ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.GlassBorder)), style.Radius, 0, 1.0f);
        }
    }
}
