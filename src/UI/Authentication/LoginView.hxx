#pragma once

#include "UI/Components/TextInput.hxx"
#include "UI/Components/Button.hxx"
#include "UI/Components/LinkText.hxx"
#include "UI/Components/SocialLinks.hxx"
#include "AuthLayout.hxx"
#include "imgui.h"

namespace UI::Authentication
{
    class LoginView
    {
    public:
        LoginView();

        void Initialize();
        void Reset();
        void Update(float deltaSeconds);
        ViewAction Draw(ImVec2 origin, float width);

        float ContentHeight() const;
        const char* Username() const { return m_Username; }

    private:
        char m_Username[64];
        char m_Password[64];

        Components::TextInput m_UsernameInput;
        Components::TextInput m_PasswordInput;
        Components::Button m_SignInButton;
        Components::LinkText m_SignUpLink;
        Components::SocialLinks m_SocialLinks;
    };
}
