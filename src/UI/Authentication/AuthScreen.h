#pragma once

#include "LoginView.h"
#include "SignUpView.h"
#include "UI/Animation/Animation.h"
#include "UI/Screens/Screen.h"
#include "imgui.h"

#include <functional>

struct ID3D11Device;

namespace UI::Authentication
{
    enum class AuthMode
    {
        Login,
        SignUp
    };

    class AuthScreen : public Screens::Screen
    {
    public:
        AuthScreen();

        void Initialize(ID3D11Device* device) override;
        void Reset();
        void Update(float deltaSeconds) override;
        void Draw(ImVec2 regionMin, ImVec2 regionSize) override;

        void SetOnAuthenticated(std::function<void(const char*)> callback) { m_OnAuthenticated = std::move(callback); }

    private:
        ImVec2 OriginFor(ImVec2 regionMin, ImVec2 regionSize, float contentHeight, float width, float offsetX) const;
        float HeightFor(AuthMode mode) const;
        ViewAction DrawMode(AuthMode mode, ImVec2 origin, float width);

        LoginView m_LoginView;
        SignUpView m_SignUpView;
        Transition m_Transition;
        AuthMode m_Mode;
        AuthMode m_PreviousMode;
        std::function<void(const char*)> m_OnAuthenticated;
    };
}
