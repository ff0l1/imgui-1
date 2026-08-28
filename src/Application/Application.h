#pragma once

#include "Window/Win32Window.h"
#include "Window/BackgroundOverlay.h"
#include "Renderer/D3D11Renderer.h"
#include "UI/Particles/SnowParticleSystem.h"
#include "UI/Authentication/AuthScreen.h"
#include "UI/Screens/LoaderScreen.h"
#include "UI/Screens/MainMenuScreen.h"
#include "UI/Components/WindowChrome.h"
#include "UI/Animation/Animation.h"

#include <string>

namespace Application
{
    class Application
    {
    public:
        Application();

        bool Initialize();
        void Run();
        void Shutdown();

    private:
        enum class ActiveScreen
        {
            Auth,
            Loader,
            MainMenu
        };

        enum class FadePhase
        {
            Idle,
            Out,
            In
        };

        enum class MenuVisibilityPhase
        {
            Hidden,
            Opening,
            Visible,
            Closing
        };

        static bool WantsDirectMenu();
        void LoadFonts();
        void RequestScreen(ActiveScreen screen, std::string username);
        void ApplyPendingScreen();
        void UpdateScreens(float deltaSeconds);
        void RenderFrame(float deltaSeconds);

        float ScreenOpacity() const;
        void UpdateMenuVisibility(float deltaSeconds);

        UI::Screens::Screen& CurrentScreen();

        Window::Win32Window m_Window;
        Window::BackgroundOverlay m_BackgroundOverlay;
        Renderer::D3D11Renderer m_Renderer;
        UI::SnowParticleSystem m_Particles;
        UI::Authentication::AuthScreen m_AuthScreen;
        UI::Screens::LoaderScreen m_LoaderScreen;
        UI::Screens::MainMenuScreen m_MainMenuScreen;
        UI::Components::WindowChrome m_WindowChrome;

        ActiveScreen m_ActiveScreen;
        ActiveScreen m_PendingScreen;
        std::string m_PendingUsername;
        FadePhase m_FadePhase;
        float m_FadeT;
        MenuVisibilityPhase m_MenuVisibilityPhase = MenuVisibilityPhase::Hidden;
        float m_MenuVisibilityT = 0.0f;
        int m_RegisteredMenuKey = VK_INSERT;
        bool m_MainMenuHasLeftPanel = false;
    };
}
