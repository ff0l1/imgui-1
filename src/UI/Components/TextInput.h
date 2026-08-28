#pragma once

#include "UI/Animation/Animation.h"
#include "imgui.h"
#include <string>

namespace UI::Components
{
    class TextInput
    {
    public:
        TextInput(std::string id, std::string placeholder, bool isPassword = false);

        void Update(float deltaSeconds);
        void Draw(ImVec2 origin, float width, char* buffer, size_t bufferSize);

        static float Height();

    private:
        std::string m_Id;
        std::string m_Placeholder;
        bool m_IsPassword;
        AnimatedFloat m_HoverT;
        AnimatedFloat m_FocusT;
    };
}
