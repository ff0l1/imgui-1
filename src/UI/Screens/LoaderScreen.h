#pragma once

#include "Screen.h"
#include "Loader/Session.h"
#include "UI/Loader/SidebarTab.h"
#include "UI/Loader/ProductsView.h"
#include "UI/Loader/AddTimeView.h"
#include "UI/Loader/AccountView.h"

#include <functional>
#include <string>

namespace UI::Screens
{
    class LoaderScreen : public Screen
    {
    public:
        LoaderScreen();

        void Initialize(ID3D11Device* device) override;
        void Update(float deltaSeconds) override;
        void Draw(ImVec2 regionMin, ImVec2 regionSize) override;

        ::Loader::Session& Session() { return m_Session; }
        void SetOnLogOut(std::function<void()> callback) { m_OnLogOut = std::move(callback); }
        void SetOnLoad(std::function<void(::Loader::ProductTier)> callback) { m_OnLoad = std::move(callback); }

    private:
        enum class Tab
        {
            Products,
            Account
        };

        void DrawSidebar(ImVec2 regionMin, ImVec2 regionSize);
        void DrawContent(ImVec2 contentMin, float contentWidth);

        ::Loader::Session m_Session;
        Tab m_ActiveTab;
        bool m_ShowAddTime;
        std::string m_LaunchStatus;

        Loader::SidebarTab m_ProductsTab;
        Loader::SidebarTab m_AccountTab;
        Loader::ProductsView m_ProductsView;
        Loader::AddTimeView m_AddTimeView;
        Loader::AccountView m_AccountView;

        std::function<void()> m_OnLogOut;
        std::function<void(::Loader::ProductTier)> m_OnLoad;
    };
}
