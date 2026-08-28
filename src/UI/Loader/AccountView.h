#pragma once

#include "Loader/Session.h"
#include "UI/Components/Button.h"
#include "UI/Components/SocialLinks.h"
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
