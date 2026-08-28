#pragma once

#include "UI/Animation/Animation.h"
#include "imgui.h"
#include <string>

namespace UI::Loader
{
    class SidebarTab
    {
    public:
        SidebarTab(std::string id, std::string glyph);

        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin, bool active);

        static float Size();

    private:
        std::string m_Id;
        std::string m_Glyph;
        AnimatedFloat m_HoverT;
        AnimatedFloat m_ActiveT;
    };
}
