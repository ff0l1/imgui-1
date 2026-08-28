#include "LoaderScreen.h"
#include "UI/Theme/Theme.h"
#include "UI/Icons/IconFont.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include <cmath>

namespace UI::Screens
{
    namespace
    {
        constexpr float kSidebarWidth = 64.0f;
        constexpr float kSidebarTopPadding = 14.0f;
        constexpr float kTabGap = 8.0f;
        constexpr float kContentPaddingX = 30.0f;
        constexpr float kContentPaddingY = 26.0f;
    }

    LoaderScreen::LoaderScreen()
        : m_ActiveTab(Tab::Products)
        , m_ShowAddTime(false)
        , m_ProductsTab("tab_products", ICON_PRODUCTS)
        , m_AccountTab("tab_account", ICON_ACCOUNT)
    {
    }

    void LoaderScreen::Initialize(ID3D11Device*)
    {
        m_AccountView.Initialize();
    }

    void LoaderScreen::Update(float deltaSeconds)
    {
        m_ProductsTab.Update(deltaSeconds);
        m_AccountTab.Update(deltaSeconds);
        m_ProductsView.Update(deltaSeconds);
        m_AddTimeView.Update(deltaSeconds);
        m_AccountView.Update(deltaSeconds);
    }

    void LoaderScreen::DrawSidebar(ImVec2 regionMin, ImVec2 regionSize)
    {
        const ThemeColors& colors = Theme::Get().Colors();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImU32 dividerColor = ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.Divider));
        const float sidebarEdge = std::floor(regionMin.x + kSidebarWidth) + 0.5f;
        const float headerEdge = std::floor(regionMin.y) + 0.5f;

        drawList->AddLine(ImVec2(regionMin.x, headerEdge),
            ImVec2(regionMin.x + regionSize.x, headerEdge), dividerColor, 1.0f);

        drawList->AddLine(ImVec2(sidebarEdge, headerEdge),
            ImVec2(sidebarEdge, regionMin.y + regionSize.y), dividerColor, 1.0f);

        const float tabSize = Loader::SidebarTab::Size();
        const float tabX = regionMin.x + (kSidebarWidth - tabSize) * 0.5f;
        float tabY = regionMin.y + kSidebarTopPadding;

        if (m_ProductsTab.Draw(ImVec2(tabX, tabY), m_ActiveTab == Tab::Products))
        {
            m_ActiveTab = Tab::Products;
            m_ShowAddTime = false;
        }
        tabY += tabSize + kTabGap;

        if (m_AccountTab.Draw(ImVec2(tabX, tabY), m_ActiveTab == Tab::Account))
            m_ActiveTab = Tab::Account;
    }

    void LoaderScreen::DrawContent(ImVec2 contentMin, float contentWidth)
    {
        if (m_ActiveTab == Tab::Account)
        {
            if (m_AccountView.Draw(contentMin, contentWidth, m_Session) && m_OnLogOut)
                m_OnLogOut();
            return;
        }

        if (m_ShowAddTime)
        {
            const Loader::AddTimeAction action = m_AddTimeView.Draw(contentMin, contentWidth);

            if (action.RedeemRequested)
            {
                if (m_Session.RedeemKey(action.Key))
                    m_ShowAddTime = false;
                else
                    m_AddTimeView.SetStatus("That key is not valid.");
            }

            if (action.BackRequested)
                m_ShowAddTime = false;

            return;
        }

        const Loader::ProductsAction action = m_ProductsView.Draw(contentMin, contentWidth, m_Session, m_LaunchStatus);
        if (action.AddTimeRequested)
        {
            m_AddTimeView.Reset();
            m_LaunchStatus.clear();
            m_ShowAddTime = true;
        }

        if (action.LoadRequested)
        {
            m_LaunchStatus = std::string("Launching ") + ::Loader::TierName(action.LoadedTier) + " build...";
            if (m_OnLoad)
                m_OnLoad(action.LoadedTier);
        }
    }

    void LoaderScreen::Draw(ImVec2 regionMin, ImVec2 regionSize)
    {
        DrawSidebar(regionMin, regionSize);

        const ImVec2 contentMin(regionMin.x + kSidebarWidth + kContentPaddingX, regionMin.y + kContentPaddingY);
        const float contentWidth = regionSize.x - kSidebarWidth - kContentPaddingX * 2.0f;

        DrawContent(contentMin, contentWidth);
    }
}
