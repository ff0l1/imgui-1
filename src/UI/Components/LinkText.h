#pragma once

#include "UI/Animation/Animation.h"
#include "imgui.h"
#include <string>

namespace UI::Components
{
    class LinkText
    {
    public:
        explicit LinkText(std::string text);

        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin, float width);

        static float Height();

    private:
        std::string m_Text;
        AnimatedFloat m_HoverT;
    };
}
