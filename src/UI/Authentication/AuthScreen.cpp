#include "AuthScreen.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"

namespace UI::Authentication
{
    namespace
    {
        constexpr float kSlideDistance = 18.0f;
    }

    AuthScreen::AuthScreen()
        : m_Transition(Theme::Get().Animation().TransitionSeconds)
        , m_Mode(AuthMode::Login)
        , m_PreviousMode(AuthMode::Login)
    {
    }

    void AuthScreen::Initialize(ID3D11Device*)
    {
        m_LoginView.Initialize();
        m_SignUpView.Initialize();
    }

    void AuthScreen::Reset()
    {
        m_LoginView.Reset();
        m_SignUpView.Reset();
        m_Mode = AuthMode::Login;
        m_PreviousMode = AuthMode::Login;
    }

    void AuthScreen::Update(float deltaSeconds)
    {
        m_Transition.Update(deltaSeconds);
        m_LoginView.Update(deltaSeconds);
        m_SignUpView.Update(deltaSeconds);
    }

    float AuthScreen::HeightFor(AuthMode mode) const
    {
        return mode == AuthMode::Login ? m_LoginView.ContentHeight() : m_SignUpView.ContentHeight();
    }

    ImVec2 AuthScreen::OriginFor(ImVec2 regionMin, ImVec2 regionSize, float contentHeight, float width, float offsetX) const
    {
        return ImVec2(
            regionMin.x + (regionSize.x - width) * 0.5f + offsetX,
            regionMin.y + (regionSize.y - contentHeight) * 0.5f);
    }

    ViewAction AuthScreen::DrawMode(AuthMode mode, ImVec2 origin, float width)
    {
        return mode == AuthMode::Login
            ? m_LoginView.Draw(origin, width)
            : m_SignUpView.Draw(origin, width);
    }

    void AuthScreen::Draw(ImVec2 regionMin, ImVec2 regionSize)
    {
        const float width = Theme::Get().Metrics().ContentWidth;
        const ImVec2 regionMax(regionMin.x + regionSize.x, regionMin.y + regionSize.y);

        ImGui::PushClipRect(regionMin, regionMax, true);

        if (m_Transition.IsActive())
        {
            const float eased = Ease::InOutCubic(m_Transition.Progress());
            const float direction = m_Transition.IsForward() ? 1.0f : -1.0f;

            ImGui::BeginDisabled(true);

            RenderOpacity::Push(1.0f - eased);
            const ImVec2 previousOrigin = OriginFor(regionMin, regionSize, HeightFor(m_PreviousMode), width,
                -direction * kSlideDistance * eased);
            DrawMode(m_PreviousMode, previousOrigin, width);
            RenderOpacity::Pop();

            RenderOpacity::Push(eased);
            const ImVec2 currentOrigin = OriginFor(regionMin, regionSize, HeightFor(m_Mode), width,
                direction * kSlideDistance * (1.0f - eased));
            DrawMode(m_Mode, currentOrigin, width);
            RenderOpacity::Pop();

            ImGui::EndDisabled();
        }
        else
        {
            const ImVec2 origin = OriginFor(regionMin, regionSize, HeightFor(m_Mode), width, 0.0f);
            const ViewAction action = DrawMode(m_Mode, origin, width);

            if (action.SwitchRequested)
            {
                m_PreviousMode = m_Mode;
                m_Mode = (m_Mode == AuthMode::Login) ? AuthMode::SignUp : AuthMode::Login;
                m_Transition.Start(m_Mode == AuthMode::SignUp);
            }
            else if (action.SubmitRequested && m_OnAuthenticated)
            {
                m_OnAuthenticated(m_Mode == AuthMode::Login
                    ? m_LoginView.Username()
                    : m_SignUpView.Username());
            }
        }

        ImGui::PopClipRect();
    }
}
