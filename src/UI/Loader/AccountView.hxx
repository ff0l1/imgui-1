#pragma once

#include "Loader/Session.hxx"
#include "UI/Components/Button.hxx"
#include "UI/Components/SocialLinks.hxx"
#include "imgui.h"

namespace UI::Loader
{
    class AccountView
    {
    public:
        AccountView();

        void Initialize();
        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin, float width, const ::Loader::Session& session);

    private:
        Components::Button m_LogOutButton;
        Components::SocialLinks m_SocialLinks;
    };
}
