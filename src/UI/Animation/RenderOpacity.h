#pragma once

#include "imgui.h"

namespace UI
{
    class RenderOpacity
    {
    public:
        static float Current();
        static void Push(float opacity);
        static void Pop();
    };

    ImVec4 ApplyOpacity(const ImVec4& color);
}
