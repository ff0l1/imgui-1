#pragma once

#include "imgui.h"

namespace UI::Components
{
    struct GlassPanelStyle
    {
        float Radius = 18.0f;
        bool Shadow = true;
        bool Sheen = true;
        bool Border = true;
        bool CustomSurface = false;
        ImVec4 SurfaceColor{};
    };

    void DrawGlassPanel(ImVec2 min, ImVec2 max, const GlassPanelStyle& style = GlassPanelStyle{});
}
