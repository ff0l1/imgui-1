#pragma once

#include "UI/Animation/Animation.h"
#include "imgui.h"
#include <string>

namespace UI::Components
{
    enum class ButtonVariant
    {
        Filled,
        Outline,
        Danger
    };

    enum class ButtonShape
    {
        Rounded,
        Pill
    };

    class Button
    {
    public:
        Button(std::string id, std::string label, ButtonVariant variant = ButtonVariant::Filled,
               ButtonShape shape = ButtonShape::Rounded);

        void SetEnabled(bool enabled) { m_Enabled = enabled; }

        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin, ImVec2 size);
        bool Draw(ImVec2 origin, float width);

        static float Height();

    private:
        std::string m_Id;
        std::string m_Label;
        ButtonVariant m_Variant;
        ButtonShape m_Shape;
        bool m_Enabled;
        AnimatedFloat m_HoverT;
        AnimatedFloat m_PressT;
    };
}
