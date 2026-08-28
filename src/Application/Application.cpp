#include "Application.h"
#include "UI/Theme/Theme.h"
#include "UI/Icons/IconFont.h"
#include "UI/Animation/RenderOpacity.h"
#include "Utilities/EmbeddedAssets.h"
#include "Resources.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <chrono>
#include <thread>
#include <Windows.h>
#include <shellapi.h>

namespace Application
{
    namespace
    {
        constexpr int kAuthWidth = 360;
        constexpr int kAuthHeight = 450;
        constexpr int kLoaderWidth = 540;
        constexpr int kLoaderHeight = 440;
        constexpr int kMainMenuWidth = 940;
        constexpr int kMainMenuHeight = 620;

        ImFont* LoadEmbeddedFont(ImGuiIO& io, int resourceId, float size, const ImWchar* ranges)
        {
            const Utilities::EmbeddedAsset asset = Utilities::LoadEmbeddedAsset(resourceId);
            if (!asset.IsValid())
                return io.Fonts->AddFontDefault();

            ImFontConfig config;
            config.FontDataOwnedByAtlas = false;
            config.OversampleH = 3;
            config.OversampleV = 2;
            config.PixelSnapH = true;

            ImFont* font = io.Fonts->AddFontFromMemoryTTF(
                const_cast<uint8_t*>(asset.Data), static_cast<int>(asset.Size), size, &config, ranges);

            return font ? font : io.Fonts->AddFontDefault();
        }
    }

    bool Application::WantsDirectMenu()
    {
        int argc = 0;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv == nullptr)
            return false;

        bool wanted = false;
        for (int i = 1; i < argc; ++i)
        {
            if (lstrcmpiW(argv[i], L"--menu") == 0)
            {
                wanted = true;
                break;
            }
        }

        LocalFree(argv);
        return wanted;
    }

    Application::Application()
        : m_WindowChrome(m_Window)
        , m_ActiveScreen(ActiveScreen::Auth)
        , m_PendingScreen(ActiveScreen::Auth)
        , m_FadePhase(FadePhase::In)
        , m_FadeT(0.0f)
    {
    }

    bool Application::Initialize()
    {
        if (!m_Window.Create(L"imgui-menu", kAuthWidth, kAuthHeight))
            return false;

        if (!m_Renderer.Initialize(m_Window.Handle()))
            return false;

        if (!m_BackgroundOverlay.Create(m_Window.Handle()))
            return false;

        m_Window.SetOnResize([this](int width, int height)
            {
                m_Renderer.ResizeSwapChain(width, height);
                m_Particles.Reset(ImVec2(static_cast<float>(width), static_cast<float>(height)));
            });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::GetStyle().AntiAliasedLines = true;
        ImGui::GetStyle().AntiAliasedFill = true;

        LoadFonts();

        ImGui_ImplWin32_Init(m_Window.Handle());
        ImGui_ImplDX11_Init(m_Renderer.Device(), m_Renderer.Context());

        m_WindowChrome.Initialize();
        m_AuthScreen.Initialize(m_Renderer.Device());
        m_LoaderScreen.Initialize(m_Renderer.Device());
        m_MainMenuScreen.Initialize(m_Renderer.Device());

        m_AuthScreen.SetOnAuthenticated([this](const char* username)
            {
                RequestScreen(ActiveScreen::Loader, username ? username : "");
            });

        m_LoaderScreen.SetOnLogOut([this]
            {
                RequestScreen(ActiveScreen::Auth, std::string());
            });

        m_LoaderScreen.SetOnLoad([this](::Loader::ProductTier)
            {
                RequestScreen(ActiveScreen::MainMenu, std::string());
            });

        m_Particles.Reset(ImVec2(static_cast<float>(kAuthWidth), static_cast<float>(kAuthHeight)));

        if (WantsDirectMenu())
        {
            m_PendingScreen = ActiveScreen::MainMenu;
            ApplyPendingScreen();
            m_FadePhase = FadePhase::Idle;
            m_FadeT = 1.0f;
        }

        m_Window.Show();
        return true;
    }

    void Application::LoadFonts()
    {
        ImGuiIO& io = ImGui::GetIO();
        UI::ThemeTypography& typography = UI::Theme::Get().MutableTypography();

        typography.Heading = LoadEmbeddedFont(io, IDR_FONT_TEXT_MEDIUM, 17.0f, nullptr);
        typography.Body = LoadEmbeddedFont(io, IDR_FONT_TEXT_REGULAR, 15.0f, nullptr);
        typography.BodyMedium = LoadEmbeddedFont(io, IDR_FONT_TEXT_MEDIUM, 15.0f, nullptr);
        typography.Small = LoadEmbeddedFont(io, IDR_FONT_TEXT_REGULAR, 15.0f, nullptr);
        typography.IconSymbol = LoadEmbeddedFont(io, IDR_FONT_ICON_SYMBOL, 21.0f, UI::Icons::SymbolRanges());
        typography.IconAction = LoadEmbeddedFont(io, IDR_FONT_ICON_SYMBOL, 19.0f, UI::Icons::SymbolRanges());
        typography.IconControl = LoadEmbeddedFont(io, IDR_FONT_ICON_SYMBOL, 15.0f, UI::Icons::SymbolRanges());
        typography.IconBrand = LoadEmbeddedFont(io, IDR_FONT_ICON_BRAND, 18.0f, UI::Icons::BrandRanges());

        io.FontDefault = typography.Body;
        io.Fonts->Build();

        if (!typography.IconAwesome
            || !typography.IconAwesome->FindGlyphNoFallback(static_cast<ImWchar>(0xf013)))
        {
            typography.IconAwesome = nullptr;
        }
    }

    UI::Screens::Screen& Application::CurrentScreen()
    {
        switch (m_ActiveScreen)
        {
        case ActiveScreen::Loader:   return m_LoaderScreen;
        case ActiveScreen::MainMenu: return m_MainMenuScreen;
        default:                     return m_AuthScreen;
        }
    }

    void Application::RequestScreen(ActiveScreen screen, std::string username)
    {
        if (m_FadePhase != FadePhase::Idle || screen == m_ActiveScreen)
            return;

        m_PendingScreen = screen;
        m_PendingUsername = std::move(username);
        m_FadePhase = FadePhase::Out;
        m_FadeT = 0.0f;
    }

    float Application::ScreenOpacity() const
    {
        switch (m_FadePhase)
        {
        case FadePhase::Out: return 1.0f - UI::Ease::InOutCubic(m_FadeT);
        case FadePhase::In:  return UI::Ease::OutCubic(m_FadeT);
        default:             return 1.0f;
        }
    }

    void Application::ApplyPendingScreen()
    {
        m_ActiveScreen = m_PendingScreen;

        if (m_ActiveScreen == ActiveScreen::Loader)
        {
            m_BackgroundOverlay.Hide();
            m_Window.SetInsertToggleEnabled(true);
            m_Window.SetOpacity(1.0f);
            m_LoaderScreen.Session().Begin(m_PendingUsername);
            m_WindowChrome.SetTitle("imgui-menu");
            m_Window.ResizeCentered(kLoaderWidth, kLoaderHeight);
        }
        else if (m_ActiveScreen == ActiveScreen::MainMenu)
        {
            m_WindowChrome.SetTitle(std::string());
            m_Window.ResizeCentered(kMainMenuWidth, kMainMenuHeight);
            const auto panels = UI::Screens::MainMenuScreen::GetPanelRects(
                ImVec2(0.0f, 0.0f), ImVec2(static_cast<float>(kMainMenuWidth), static_cast<float>(kMainMenuHeight)));
            const auto rect = [](ImVec2 min, ImVec2 max)
                {
                    return RECT{ static_cast<LONG>(min.x), static_cast<LONG>(min.y),
                        static_cast<LONG>(max.x), static_cast<LONG>(max.y) };
                };
            m_Window.SetIslandRegion({ rect(panels.TopMin, panels.TopMax), rect(panels.MainMin, panels.MainMax) },
                static_cast<int>(UI::Screens::MainMenuScreen::PanelRadius()));
            m_Window.SetDragRegion(RECT{
                static_cast<LONG>(panels.MainMin.x), static_cast<LONG>(panels.MainMin.y),
                static_cast<LONG>(panels.MainMax.x - 48.0f), static_cast<LONG>(panels.MainMin.y + 64.0f)
            });
            m_RegisteredMenuKey = m_MainMenuScreen.UsesNativeKeyboardHotkey()
                ? m_MainMenuScreen.MenuVirtualKey() : 0;
            m_Window.SetMenuToggleVirtualKey(m_RegisteredMenuKey);
            m_Window.SetInsertToggleEnabled(m_RegisteredMenuKey != 0);
            m_MenuVisibilityPhase = MenuVisibilityPhase::Visible;
            m_MenuVisibilityT = 1.0f;
            m_Window.SetOpacity(1.0f);
            m_BackgroundOverlay.SetOpacity(1.0f);
            m_BackgroundOverlay.Show();
            SetWindowPos(m_Window.Handle(), HWND_TOPMOST, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else
        {
            m_Window.ClearRegion();
            m_Window.ClearDragRegion();
            m_Window.SetInsertToggleEnabled(false);
            m_Window.SetOpacity(1.0f);
            m_MenuVisibilityPhase = MenuVisibilityPhase::Hidden;
            m_BackgroundOverlay.Hide();
            m_LoaderScreen.Session().End();
            m_AuthScreen.Reset();
            m_WindowChrome.SetTitle(std::string());
            m_Window.ResizeCentered(kAuthWidth, kAuthHeight);
        }
    }

    void Application::UpdateMenuVisibility(float deltaSeconds)
    {
        if (m_ActiveScreen != ActiveScreen::MainMenu)
            return;

        const int menuKey = m_MainMenuScreen.UsesNativeKeyboardHotkey()
            ? m_MainMenuScreen.MenuVirtualKey() : 0;
        if (menuKey != m_RegisteredMenuKey)
        {
            m_RegisteredMenuKey = menuKey;
            m_Window.SetMenuToggleVirtualKey(menuKey);
        }

        if ((m_Window.ConsumeInsertToggleRequest() || m_MainMenuScreen.ConsumeToggleRequest())
            && m_MenuVisibilityPhase != MenuVisibilityPhase::Opening
            && m_MenuVisibilityPhase != MenuVisibilityPhase::Closing)
        {
            m_MenuVisibilityT = 0.0f;
            if (m_MenuVisibilityPhase == MenuVisibilityPhase::Visible)
            {
                m_MenuVisibilityPhase = MenuVisibilityPhase::Closing;
            }
            else
            {
                m_MenuVisibilityPhase = MenuVisibilityPhase::Opening;
                m_BackgroundOverlay.SetOpacity(0.0f);
                m_Window.SetOpacity(0.0f);
                m_BackgroundOverlay.Show();
                m_Window.SetVisible(true);
                SetWindowPos(m_Window.Handle(), HWND_TOPMOST, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
            }
        }

        if (m_MenuVisibilityPhase != MenuVisibilityPhase::Opening
            && m_MenuVisibilityPhase != MenuVisibilityPhase::Closing)
            return;

        constexpr float kVisibilitySeconds = 0.14f;
        m_MenuVisibilityT = (std::min)(1.0f, m_MenuVisibilityT + deltaSeconds / kVisibilitySeconds);
        const float t = m_MenuVisibilityT;
        const float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

        if (m_MenuVisibilityPhase == MenuVisibilityPhase::Opening)
        {
            const float menuT = (std::max)(0.0f, (t - 0.08f) / 0.92f);
            const float menuEase = 1.0f - (1.0f - menuT) * (1.0f - menuT) * (1.0f - menuT);
            m_BackgroundOverlay.SetOpacity(eased);
            m_Window.SetOpacity(menuEase);
            if (t >= 1.0f)
                m_MenuVisibilityPhase = MenuVisibilityPhase::Visible;
        }
        else
        {
            const float opacity = 1.0f - eased;
            m_Window.SetOpacity(opacity);
            m_BackgroundOverlay.SetOpacity(opacity);
            if (t >= 1.0f)
            {
                m_MenuVisibilityPhase = MenuVisibilityPhase::Hidden;
                m_Window.SetVisible(false);
                m_BackgroundOverlay.Hide();
            }
        }
    }

    void Application::UpdateScreens(float deltaSeconds)
    {
        const UI::ThemeAnimation& animation = UI::Theme::Get().Animation();

        switch (m_FadePhase)
        {
        case FadePhase::Out:
            m_FadeT += deltaSeconds / animation.ScreenOutSeconds;
            if (m_FadeT >= 1.0f)
            {
                ApplyPendingScreen();
                m_FadePhase = FadePhase::In;
                m_FadeT = 0.0f;
            }
            break;

        case FadePhase::In:
            m_FadeT += deltaSeconds / animation.ScreenInSeconds;
            if (m_FadeT >= 1.0f)
            {
                m_FadeT = 1.0f;
                m_FadePhase = FadePhase::Idle;
            }
            break;

        case FadePhase::Idle:
        default:
            break;
        }

        m_Particles.Update(deltaSeconds);
        UpdateMenuVisibility(deltaSeconds);
        if (m_ActiveScreen == ActiveScreen::MainMenu)
            m_BackgroundOverlay.Update((std::min)(deltaSeconds, 0.05f));
        m_WindowChrome.Update(deltaSeconds);
        m_AuthScreen.Update(deltaSeconds);
        m_LoaderScreen.Update(deltaSeconds);
        m_MainMenuScreen.Update(deltaSeconds);
    }

    void Application::Run()
    {
        auto previousTime = std::chrono::steady_clock::now();
        auto nextFrame = previousTime;

        while (m_Window.PumpMessages())
        {
            const auto currentTime = std::chrono::steady_clock::now();
            const float deltaSeconds = std::chrono::duration<float>(currentTime - previousTime).count();
            previousTime = currentTime;

            if (m_Window.IsMinimized())
            {
                Sleep(10);
                continue;
            }

            RenderFrame(deltaSeconds);

            if (m_ActiveScreen == ActiveScreen::MainMenu && m_MainMenuScreen.IsFpsCapEnabled())
            {
                const auto frameDuration = std::chrono::duration<double>(1.0 / m_MainMenuScreen.FpsCap());
                nextFrame = (std::max)(
                    nextFrame + std::chrono::duration_cast<std::chrono::steady_clock::duration>(frameDuration),
                    currentTime);
                std::this_thread::sleep_until(nextFrame);
            }
            else
            {
                nextFrame = currentTime;
            }
        }
    }

    void Application::RenderFrame(float deltaSeconds)
    {
        const UI::ThemeColors& colors = UI::Theme::Get().Colors();
        const UI::ThemeMetrics& metrics = UI::Theme::Get().Metrics();

        UpdateScreens(deltaSeconds);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (m_ActiveScreen == ActiveScreen::MainMenu
            && m_MainMenuHasLeftPanel != m_MainMenuScreen.HasLeftPanel())
        {
            const auto panels = UI::Screens::MainMenuScreen::GetPanelRects(
                ImVec2(0.0f, 0.0f), ImVec2(static_cast<float>(m_Window.Width()), static_cast<float>(m_Window.Height())));
            const auto rect = [](ImVec2 min, ImVec2 max)
                {
                    return RECT{ static_cast<LONG>(min.x), static_cast<LONG>(min.y),
                        static_cast<LONG>(max.x), static_cast<LONG>(max.y) };
                };
            std::vector<RECT> islands{ rect(panels.TopMin, panels.TopMax), rect(panels.MainMin, panels.MainMax) };
            if (m_MainMenuScreen.HasLeftPanel())
                islands.push_back(rect(panels.LeftMin, panels.LeftMax));
            m_Window.SetIslandRegion(islands, static_cast<int>(UI::Screens::MainMenuScreen::PanelRadius()));
            m_MainMenuHasLeftPanel = m_MainMenuScreen.HasLeftPanel();
        }

        const float width = static_cast<float>(m_Window.Width());
        const float height = static_cast<float>(m_Window.Height());

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(width, height));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        constexpr ImGuiWindowFlags rootFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("##root", nullptr, rootFlags);
        ImGui::PopStyleVar(3);

        const float fade = ScreenOpacity();
        UI::RenderOpacity::Push(fade);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);

        if (m_ActiveScreen != ActiveScreen::MainMenu)
            m_Particles.Render(ImGui::GetWindowDrawList());

        if (m_ActiveScreen == ActiveScreen::Loader)
        {
            const ImVec2 regionMin(0.0f, metrics.TitleBarHeight);
            CurrentScreen().Draw(regionMin, ImVec2(width, height - metrics.TitleBarHeight));
        }
        else
        {
            CurrentScreen().Draw(ImVec2(0.0f, 0.0f), ImVec2(width, height));
        }

        if (m_ActiveScreen == ActiveScreen::MainMenu)
        {
            const auto panels = UI::Screens::MainMenuScreen::GetPanelRects(
                ImVec2(0.0f, 0.0f), ImVec2(width, height));
            m_WindowChrome.DrawInHeader(
                panels.MainMin, ImVec2(panels.MainMax.x, panels.MainMin.y + 64.0f));
        }
        else
        {
            m_WindowChrome.Draw(ImVec2(width, height));
        }

        ImGui::PopStyleVar();
        UI::RenderOpacity::Pop();

        ImGui::End();

        ImGui::Render();

        m_Renderer.BeginFrame(colors.Background);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        const bool useVSync = m_ActiveScreen != ActiveScreen::MainMenu || !m_MainMenuScreen.IsFpsCapEnabled();
        m_Renderer.EndFrame(useVSync);
    }

    void Application::Shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}
