#pragma once

#include "UI/Animation/Animation.h"
#include "imgui.h"
#include <string>

struct ImFont;

namespace UI::Components
{
    struct IconButtonStyle
    {
        float Size;
        bool Circular;
        bool FilledSurface;
        ImVec4 IdleColor;
        ImVec4 HoverColor;
        ImVec4 PressedColor;
        ImVec4 HoverSurface;
    };

    class IconButton
    {
    public:
        IconButton(std::string id, std::string glyph, ImFont* font, const IconButtonStyle& style);

        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin);

        float Size() const { return m_Style.Size; }

    private:
        std::string m_Id;
        std::string m_Glyph;
        ImFont* m_Font;
        IconButtonStyle m_Style;
        AnimatedFloat m_HoverT;
    };
}
