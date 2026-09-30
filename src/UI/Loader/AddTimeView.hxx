#pragma once

#include "UI/Components/Button.hxx"
#include "UI/Components/TextInput.hxx"
#include "UI/Components/LinkText.hxx"
#include "imgui.h"

#include <string>

namespace UI::Loader
{
    struct AddTimeAction
    {
        bool BackRequested = false;
        bool RedeemRequested = false;
        std::string Key;
    };

    class AddTimeView
    {
    public:
        AddTimeView();

        void Reset();
        void SetStatus(std::string message);
        void Update(float deltaSeconds);
        AddTimeAction Draw(ImVec2 origin, float width);

    private:
        char m_Key[64];
        std::string m_Status;

        Components::TextInput m_KeyInput;
        Components::Button m_RedeemButton;
        Components::LinkText m_BackLink;
    };
}
