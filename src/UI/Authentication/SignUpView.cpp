#include "SignUpView.h"
#include "AuthLayout.h"

#include <cstring>

namespace UI::Authentication
{
    SignUpView::SignUpView()
        : m_Username{}
        , m_Password{}
        , m_Key{}
        , m_UsernameInput("signup_username", "Username")
        , m_PasswordInput("signup_password", "Password", true)
        , m_KeyInput("signup_key", "License key")
        , m_CreateAccountButton("signup_create", "Create Account")
        , m_SignInLink("Sign In")
        , m_SocialLinks("signup")
    {
    }

    void SignUpView::Initialize()
    {
        m_SocialLinks.Initialize();
    }

    void SignUpView::Reset()
    {
        std::memset(m_Username, 0, sizeof(m_Username));
        std::memset(m_Password, 0, sizeof(m_Password));
        std::memset(m_Key, 0, sizeof(m_Key));
    }

    void SignUpView::Update(float deltaSeconds)
    {
        m_UsernameInput.Update(deltaSeconds);
        m_PasswordInput.Update(deltaSeconds);
        m_KeyInput.Update(deltaSeconds);
        m_CreateAccountButton.Update(deltaSeconds);
        m_SignInLink.Update(deltaSeconds);
        m_SocialLinks.Update(deltaSeconds);

        m_CreateAccountButton.SetEnabled(m_Username[0] != '\0' && m_Password[0] != '\0' && m_Key[0] != '\0');
    }

    float SignUpView::ContentHeight() const
    {
        return Layout::LogoHeight()
            + Layout::LogoToFields
            + Components::TextInput::Height() * 3.0f + Layout::FieldGap * 2.0f
            + Layout::FieldsToButton
            + Components::Button::Height()
            + Layout::ButtonToLink
            + Components::LinkText::Height()
            + Layout::LinkToSocial
            + Components::SocialLinks::Height();
    }

    ViewAction SignUpView::Draw(ImVec2 origin, float width)
    {
        ViewAction action;
        const float inputHeight = Components::TextInput::Height();
        float y = origin.y;

        Layout::DrawBrand(ImVec2(origin.x, y), width);
        y += Layout::LogoHeight() + Layout::LogoToFields;

        m_UsernameInput.Draw(ImVec2(origin.x, y), width, m_Username, sizeof(m_Username));
        y += inputHeight + Layout::FieldGap;

        m_PasswordInput.Draw(ImVec2(origin.x, y), width, m_Password, sizeof(m_Password));
        y += inputHeight + Layout::FieldGap;

        m_KeyInput.Draw(ImVec2(origin.x, y), width, m_Key, sizeof(m_Key));
        y += inputHeight + Layout::FieldsToButton;

        action.SubmitRequested = m_CreateAccountButton.Draw(ImVec2(origin.x, y), width);
        y += Components::Button::Height() + Layout::ButtonToLink;

        action.SwitchRequested = m_SignInLink.Draw(ImVec2(origin.x, y), width);
        y += Components::LinkText::Height() + Layout::LinkToSocial;

        m_SocialLinks.Draw(ImVec2(origin.x, y), width);

        return action;
    }
}
