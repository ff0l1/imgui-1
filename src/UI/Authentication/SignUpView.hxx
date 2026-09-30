#pragma once

#include "UI/Components/TextInput.hxx"
#include "UI/Components/Button.hxx"
#include "UI/Components/LinkText.hxx"
#include "UI/Components/SocialLinks.hxx"
#include "AuthLayout.hxx"
#include "imgui.h"

namespace UI::Authentication
{
    class SignUpView
    {
    public:
        SignUpView();

        void Initialize();
        void Reset();
        void Update(float deltaSeconds);
        ViewAction Draw(ImVec2 origin, float width);

        float ContentHeight() const;
        const char* Username() const { return m_Username; }

    private:
        char m_Username[64];
        char m_Password[64];
        char m_Key[64];

        Components::TextInput m_UsernameInput;
        Components::TextInput m_PasswordInput;
        Components::TextInput m_KeyInput;
        Components::Button m_CreateAccountButton;
        Components::LinkText m_SignInLink;
        Components::SocialLinks m_SocialLinks;
    };
}
