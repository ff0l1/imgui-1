#include "LoginView.h"
#include "AuthLayout.h"

#include <cstring>

namespace UI::Authentication
{
    LoginView::LoginView()
        : m_Username{}
        , m_Password{}
        , m_UsernameInput("login_username", "Username")
        , m_PasswordInput("login_password", "Password", true)
        , m_SignInButton("login_signin", "Sign In")
        , m_SignUpLink("Sign Up")
        , m_SocialLinks("login")
    {
    }

    void LoginView::Initialize()
    {
        m_SocialLinks.Initialize();
    }

    void LoginView::Reset()
    {
        std::memset(m_Username, 0, sizeof(m_Username));
        std::memset(m_Password, 0, sizeof(m_Password));
    }

    void LoginView::Update(float deltaSeconds)
    {
        m_UsernameInput.Update(deltaSeconds);
        m_PasswordInput.Update(deltaSeconds);
        m_SignInButton.Update(deltaSeconds);
        m_SignUpLink.Update(deltaSeconds);
        m_SocialLinks.Update(deltaSeconds);

        m_SignInButton.SetEnabled(m_Username[0] != '\0' && m_Password[0] != '\0');
    }

    float LoginView::ContentHeight() const
    {
        return Layout::LogoHeight()
            + Layout::LogoToFields
            + Components::TextInput::Height() * 2.0f + Layout::FieldGap
            + Layout::FieldsToButton
            + Components::Button::Height()
            + Layout::ButtonToLink
            + Components::LinkText::Height()
            + Layout::LinkToSocial
            + Components::SocialLinks::Height();
    }

    ViewAction LoginView::Draw(ImVec2 origin, float width)
    {
        ViewAction action;
        const float inputHeight = Components::TextInput::Height();
        float y = origin.y;

        Layout::DrawBrand(ImVec2(origin.x, y), width);
        y += Layout::LogoHeight() + Layout::LogoToFields;

        m_UsernameInput.Draw(ImVec2(origin.x, y), width, m_Username, sizeof(m_Username));
        y += inputHeight + Layout::FieldGap;

        m_PasswordInput.Draw(ImVec2(origin.x, y), width, m_Password, sizeof(m_Password));
        y += inputHeight + Layout::FieldsToButton;

        action.SubmitRequested = m_SignInButton.Draw(ImVec2(origin.x, y), width);
        y += Components::Button::Height() + Layout::ButtonToLink;

        action.SwitchRequested = m_SignUpLink.Draw(ImVec2(origin.x, y), width);
        y += Components::LinkText::Height() + Layout::LinkToSocial;

        m_SocialLinks.Draw(ImVec2(origin.x, y), width);

        return action;
    }
}
