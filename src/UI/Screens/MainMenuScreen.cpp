#include "MainMenuScreen.h"
#include "UI/Menu/MenuWidgets.h"
#include "UI/Components/GlassPanel.h"
#include "UI/Components/Layout.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"
#include "Utilities/EmbeddedAssets.h"
#include "Utilities/ImageDecoder.h"
#include "Resources.h"

#include "imgui.h"
#include <array>
#include <cmath>
#include <Xinput.h>

namespace UI::Screens
{
    using namespace UI::Menu;


    void MainMenuScreen::Initialize(ID3D11Device* device)
    {
        const Utilities::EmbeddedAsset gearAsset = Utilities::LoadEmbeddedAsset(IDR_IMAGE_SETTINGS_GEAR);
        Utilities::DecodedImage gear = Utilities::DecodeImage(gearAsset.Data, gearAsset.Size);
        if (gear.IsValid())
        {
            Utilities::ConvertToWhiteSilhouette(gear);
            Utilities::ResampleImage(gear, 20, 20);
            m_SettingsGear.Create(device, gear.Frames.front().Pixels.data(), gear.Width, gear.Height);
        }

        const auto loadIcon = [device](int resourceId, int pixelSize, Renderer::Texture& texture)
            {
                const Utilities::EmbeddedAsset asset = Utilities::LoadEmbeddedAsset(resourceId);
                Utilities::DecodedImage image = Utilities::DecodeImage(asset.Data, asset.Size);
                if (image.IsValid())
                {
                    Utilities::ConvertToWhiteSilhouette(image);
                    Utilities::ResampleImage(image, pixelSize, pixelSize);
                    texture.Create(device, image.Frames.front().Pixels.data(), image.Width, image.Height);
                }
            };
        loadIcon(IDR_IMAGE_MISC_GLOBE, 20, m_MiscGlobe);
        loadIcon(IDR_IMAGE_EXPLOITS_BOMB, 18, m_ExploitsBomb);
        loadIcon(IDR_IMAGE_VISUALS_EYE, 20, m_VisualsEye);
        loadIcon(IDR_IMAGE_VISUALS_BODY, 18, m_VisualsBody);
        loadIcon(IDR_IMAGE_VISUALS_INFO, 18, m_VisualsInfo);
        loadIcon(IDR_IMAGE_VISUALS_LAYER, 18, m_VisualsOverlay);
        loadIcon(IDR_IMAGE_VISUALS_MAP, 18, m_VisualsMap);
        loadIcon(IDR_IMAGE_AIMBOT_CROSSHAIRS, 20, m_AimbotCrosshairs);
        loadIcon(IDR_IMAGE_AIMBOT_GLOBAL, 18, m_AimbotGlobal);
        loadIcon(IDR_IMAGE_AIMBOT_SILENT, 18, m_AimbotSilent);
        loadIcon(IDR_IMAGE_AIMBOT_TRIGGER, 18, m_AimbotTrigger);
        const auto loadWeapon = [device](int resourceId, Renderer::Texture& texture)
            {
                const Utilities::EmbeddedAsset asset = Utilities::LoadEmbeddedAsset(resourceId);
                Utilities::DecodedImage image = Utilities::DecodeImage(asset.Data, asset.Size);
                if (image.IsValid())
                {
                    Utilities::ResampleImage(image, 72, 72);
                    texture.Create(device, image.Frames.front().Pixels.data(), image.Width, image.Height);
                }
            };
        loadWeapon(IDR_IMAGE_WEAPON_SHOTGUN, m_WeaponShotgun);
        loadWeapon(IDR_IMAGE_WEAPON_AR, m_WeaponAr);
        loadWeapon(IDR_IMAGE_WEAPON_SMG, m_WeaponSmg);
        loadWeapon(IDR_IMAGE_WEAPON_PISTOL, m_WeaponPistol);
        loadWeapon(IDR_IMAGE_WEAPON_SNIPER, m_WeaponSniper);
    }

    void MainMenuScreen::Update(float deltaSeconds)
    {
        const auto approach = [deltaSeconds](float current, float target, float speed)
            {
                const float step = speed * deltaSeconds;
                if (current < target)
                    return (std::min)(target, current + step);
                return (std::max)(target, current - step);
            };
        m_StreamProofT = approach(m_StreamProofT, m_StreamProof ? 1.0f : 0.0f, 8.5f);
        m_WatermarkT = approach(m_WatermarkT, m_ShowWatermark ? 1.0f : 0.0f, 8.5f);
        m_FpsCapT = approach(m_FpsCapT, m_EnableFpsCap ? 1.0f : 0.0f, 8.5f);
        m_FovChangerT = approach(m_FovChangerT, m_FovChanger ? 1.0f : 0.0f, 8.5f);
        m_FovZoomT = approach(m_FovZoomT, m_FovZoom ? 1.0f : 0.0f, 8.5f);
        m_ForceTPoseT = approach(m_ForceTPoseT, m_ForceTPose ? 1.0f : 0.0f, 8.5f);
        m_InstantInteractionT = approach(m_InstantInteractionT, m_InstantInteraction ? 1.0f : 0.0f, 8.5f);
        m_SpinbotT = approach(m_SpinbotT, m_Spinbot ? 1.0f : 0.0f, 8.5f);
        m_PlayerSizeT = approach(m_PlayerSizeT, m_PlayerSize ? 1.0f : 0.0f, 8.5f);
        m_TinyPlayerT = approach(m_TinyPlayerT, m_TinyPlayer ? 1.0f : 0.0f, 8.5f);
        m_PageTransitionT = (std::min)(1.0f, m_PageTransitionT + deltaSeconds / 0.12f);

        XINPUT_STATE state{};
        const WORD buttons = XInputGetState(0, &state) == ERROR_SUCCESS ? state.Gamepad.wButtons : 0;
        if (m_CapturingTriggerBind != 0)
        {
            bool anyKeyboardDown = false;
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    anyKeyboardDown = true;
                    break;
                }
            }
            if (!m_TriggerCaptureArmed)
            {
                m_TriggerCaptureArmed = !anyKeyboardDown && buttons == 0;
                return;
            }
            int& virtualKey = m_CapturingTriggerBind == 1
                ? m_Triggerbot.PrimaryVirtualKey : m_Triggerbot.SecondaryVirtualKey;
            WORD& gamepadButton = m_CapturingTriggerBind == 1
                ? m_Triggerbot.PrimaryGamepadButton : m_Triggerbot.SecondaryGamepadButton;
            if (buttons != 0)
            {
                gamepadButton = FirstGamepadButton(buttons);
                virtualKey = 0;
                m_CapturingTriggerBind = 0;
                return;
            }
            for (int candidate = 1; candidate < 256; ++candidate)
            {
                if ((GetAsyncKeyState(candidate) & 0x8000) != 0)
                {
                    virtualKey = candidate;
                    gamepadButton = 0;
                    m_CapturingTriggerBind = 0;
                    return;
                }
            }
            return;
        }
        if (m_CapturingAimbotBind != 0)
        {
            bool anyKeyboardDown = false;
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    anyKeyboardDown = true;
                    break;
                }
            }
            if (!m_AimbotCaptureArmed)
            {
                m_AimbotCaptureArmed = !anyKeyboardDown && buttons == 0;
                return;
            }
            int& virtualKey = m_CapturingAimbotBind == 1
                ? m_GlobalAimbot.PrimaryVirtualKey : m_GlobalAimbot.SecondaryVirtualKey;
            WORD& gamepadButton = m_CapturingAimbotBind == 1
                ? m_GlobalAimbot.PrimaryGamepadButton : m_GlobalAimbot.SecondaryGamepadButton;
            if (buttons != 0)
            {
                gamepadButton = FirstGamepadButton(buttons);
                virtualKey = 0;
                m_CapturingAimbotBind = 0;
                return;
            }
            for (int candidate = 1; candidate < 256; ++candidate)
            {
                if ((GetAsyncKeyState(candidate) & 0x8000) != 0)
                {
                    virtualKey = candidate;
                    gamepadButton = 0;
                    m_CapturingAimbotBind = 0;
                    return;
                }
            }
            return;
        }
        if (m_CapturingZoomKey)
        {
            bool anyKeyboardDown = false;
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    anyKeyboardDown = true;
                    break;
                }
            }
            if (!m_ZoomCaptureArmed)
            {
                m_ZoomCaptureArmed = !anyKeyboardDown && buttons == 0;
                return;
            }
            if (buttons != 0)
            {
                m_ZoomGamepadButton = FirstGamepadButton(buttons);
                m_ZoomVirtualKey = 0;
                m_CapturingZoomKey = false;
                return;
            }
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    m_ZoomVirtualKey = virtualKey;
                    m_ZoomGamepadButton = 0;
                    m_CapturingZoomKey = false;
                    return;
                }
            }
            return;
        }
        if (m_CapturingMenuKey)
        {
            bool anyKeyboardDown = false;
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    anyKeyboardDown = true;
                    break;
                }
            }
            if (!m_CaptureArmed)
            {
                m_CaptureArmed = !anyKeyboardDown && buttons == 0;
                return;
            }

            if (buttons != 0)
            {
                m_MenuGamepadButton = FirstGamepadButton(buttons);
                m_MenuVirtualKey = 0;
                m_CapturingMenuKey = false;
                m_BindingWasDown = true;
                return;
            }
            for (int virtualKey = 1; virtualKey < 256; ++virtualKey)
            {
                if ((GetAsyncKeyState(virtualKey) & 0x8000) != 0)
                {
                    m_MenuVirtualKey = virtualKey;
                    m_MenuGamepadButton = 0;
                    m_CapturingMenuKey = false;
                    m_BindingWasDown = true;
                    return;
                }
            }
            return;
        }

        const bool bindingDown = m_MenuGamepadButton != 0
            ? (buttons & m_MenuGamepadButton) != 0
            : !UsesNativeKeyboardHotkey() && m_MenuVirtualKey != 0
                && (GetAsyncKeyState(m_MenuVirtualKey) & 0x8000) != 0;
        if (bindingDown && !m_BindingWasDown)
            m_ToggleRequested = true;
        m_BindingWasDown = bindingDown;
    }

    bool MainMenuScreen::ConsumeToggleRequest()
    {
        const bool requested = m_ToggleRequested;
        m_ToggleRequested = false;
        return requested;
    }

    bool MainMenuScreen::UsesNativeKeyboardHotkey() const
    {
        return m_MenuGamepadButton == 0 && m_MenuVirtualKey > VK_XBUTTON2;
    }

    MainMenuScreen::PanelRects MainMenuScreen::GetPanelRects(ImVec2 regionMin, ImVec2 regionSize)
    {
        const ImVec2 canvasMin(
            regionMin.x + (regionSize.x - kMenuWidth) * 0.5f,
            regionMin.y + (regionSize.y - kMenuHeight) * 0.5f);
        const ImVec2 contentMin(canvasMin.x + kOuterMargin, canvasMin.y + kOuterMargin);
        const ImVec2 contentMax(canvasMin.x + kMenuWidth - kOuterMargin, canvasMin.y + kMenuHeight - kOuterMargin);
        const float belowTopBar = contentMin.y + kTopBarHeight + kPanelGap;
        return {
            ImVec2(contentMin.x + kLeftPanelWidth + kPanelGap, contentMin.y),
                ImVec2(contentMax.x, contentMin.y + kTopBarHeight),
            ImVec2(contentMin.x, belowTopBar), ImVec2(contentMin.x + kLeftPanelWidth, contentMax.y),
            ImVec2(contentMin.x + kLeftPanelWidth + kPanelGap, belowTopBar), contentMax
        };
    }

    float MainMenuScreen::PanelRadius()
    {
        return kPanelRadius;
    }

    void MainMenuScreen::Draw(ImVec2 regionMin, ImVec2 regionSize)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        const PanelRects panels = GetPanelRects(regionMin, regionSize);
        Components::GlassPanelStyle topBarStyle;
        topBarStyle.Radius = kTopBarRadius;
        topBarStyle.Shadow = false;
        topBarStyle.Sheen = false;
        topBarStyle.Border = false;
        topBarStyle.CustomSurface = true;
        topBarStyle.SurfaceColor = kTopSurface;
        Components::DrawGlassPanel(
            panels.TopMin, panels.TopMax, topBarStyle);

        Components::GlassPanelStyle panelStyle;
        panelStyle.Radius = kPanelRadius;
        panelStyle.Shadow = false;
        panelStyle.Sheen = false;
        panelStyle.Border = false;
        panelStyle.CustomSurface = true;
        panelStyle.SurfaceColor = kMainSurface;
        const ImVec2 mainMin = panels.MainMin;
        Components::DrawGlassPanel(mainMin, panels.MainMax, panelStyle);

        Components::DrawTextAt(
            ImVec2(mainMin.x + kBrandPadding, mainMin.y + kBrandPadding),
            "imgui-menu", typography.BodyMedium, colors.TextPrimary);

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 aimbotMin(panels.TopMin.x + 10.0f, panels.TopMin.y + 8.0f);
        const float aimbotCollapsedWidth = 42.0f;
        const float aimbotExpandedWidth = 102.0f;
        const ImVec2 initialHitMax(aimbotMin.x + aimbotExpandedWidth, panels.TopMax.y - 8.0f);
        const float previousAimbotEase = m_AimbotHoverT * m_AimbotHoverT * (3.0f - 2.0f * m_AimbotHoverT);
        const float previousAimbotWidth = aimbotCollapsedWidth
            + previousAimbotEase * (aimbotExpandedWidth - aimbotCollapsedWidth);
        ImGui::SetCursorScreenPos(aimbotMin);
        if (ImGui::InvisibleButton("##aimbot_tab", ImVec2(previousAimbotWidth,
            initialHitMax.y - aimbotMin.y)))
        {
            if (m_ActiveTab != Tab::Aimbot)
                m_PageTransitionT = 0.0f;
            m_ActiveTab = Tab::Aimbot;
        }
        const bool aimbotHovered = ImGui::IsItemHovered();
        m_AimbotHoverT += ((aimbotHovered ? 1.0f : 0.0f) - m_AimbotHoverT)
            * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 18.0f));
        const float aimbotEase = m_AimbotHoverT * m_AimbotHoverT * (3.0f - 2.0f * m_AimbotHoverT);
        const float aimbotWidth = aimbotCollapsedWidth
            + aimbotEase * (aimbotExpandedWidth - aimbotCollapsedWidth);
        const ImVec2 aimbotMax(aimbotMin.x + aimbotWidth, initialHitMax.y);
        if (aimbotEase > 0.01f)
            drawList->AddRectFilled(aimbotMin, aimbotMax,
                Color(255, 255, 255, static_cast<int>(aimbotEase * 20.0f)), 12.0f);
        if (m_AimbotCrosshairs.IsValid())
        {
            const ImVec2 iconMin(std::floor(aimbotMin.x + 12.0f),
                std::floor((aimbotMin.y + aimbotMax.y) * 0.5f - 10.0f));
            const int alpha = m_ActiveTab == Tab::Aimbot ? 255 : 190 + static_cast<int>(aimbotEase * 45.0f);
            drawList->AddImage(reinterpret_cast<ImTextureID>(m_AimbotCrosshairs.View()),
                iconMin, ImVec2(iconMin.x + 20.0f, iconMin.y + 20.0f),
                ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color(255, 255, 255, alpha));
        }
        if (aimbotEase > 0.04f)
        {
            ImVec4 textColor = colors.TextPrimary;
            textColor.w *= aimbotEase;
            const float h = Components::TextHeight(typography.BodyMedium);
            Components::DrawTextAt(ImVec2(aimbotMin.x + 40.0f,
                (aimbotMin.y + aimbotMax.y - h) * 0.5f), "Aimbot", typography.BodyMedium, textColor);
        }

        const ImVec2 visualsMin(aimbotMin.x + aimbotWidth, aimbotMin.y);
        const float visualsCollapsedWidth = 42.0f;
        const float visualsExpandedWidth = 104.0f;
        const float previousVisualsEase = m_VisualsHoverT * m_VisualsHoverT * (3.0f - 2.0f * m_VisualsHoverT);
        const float previousVisualsWidth = visualsCollapsedWidth
            + previousVisualsEase * (visualsExpandedWidth - visualsCollapsedWidth);
        ImGui::SetCursorScreenPos(visualsMin);
        if (ImGui::InvisibleButton("##visuals_tab", ImVec2(previousVisualsWidth, initialHitMax.y - visualsMin.y)))
        {
            if (m_ActiveTab != Tab::Visuals)
                m_PageTransitionT = 0.0f;
            m_ActiveTab = Tab::Visuals;
        }
        const bool visualsHovered = ImGui::IsItemHovered();
        m_VisualsHoverT += ((visualsHovered ? 1.0f : 0.0f) - m_VisualsHoverT)
            * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 18.0f));
        const float visualsEase = m_VisualsHoverT * m_VisualsHoverT * (3.0f - 2.0f * m_VisualsHoverT);
        const float visualsWidth = visualsCollapsedWidth
            + visualsEase * (visualsExpandedWidth - visualsCollapsedWidth);
        const ImVec2 visualsMax(visualsMin.x + visualsWidth, initialHitMax.y);
        if (visualsEase > 0.01f)
            drawList->AddRectFilled(visualsMin, visualsMax,
                Color(255, 255, 255, static_cast<int>(visualsEase * 20.0f)), 12.0f);
        if (m_VisualsEye.IsValid())
        {
            const ImVec2 iconMin(std::floor(visualsMin.x + 12.0f),
                std::floor((visualsMin.y + visualsMax.y) * 0.5f - 10.0f));
            const int alpha = m_ActiveTab == Tab::Visuals ? 255 : 190 + static_cast<int>(visualsEase * 45.0f);
            drawList->AddImage(reinterpret_cast<ImTextureID>(m_VisualsEye.View()),
                iconMin, ImVec2(iconMin.x + 20.0f, iconMin.y + 20.0f),
                ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color(255, 255, 255, alpha));
        }
        if (visualsEase > 0.04f)
        {
            ImVec4 textColor = colors.TextPrimary;
            textColor.w *= visualsEase;
            const float h = Components::TextHeight(typography.BodyMedium);
            Components::DrawTextAt(ImVec2(visualsMin.x + 40.0f,
                (visualsMin.y + visualsMax.y - h) * 0.5f), "Visuals", typography.BodyMedium, textColor);
        }

        const ImVec2 miscMin(visualsMin.x + visualsWidth, visualsMin.y);
        const float miscCollapsedWidth = 42.0f;
        const float miscExpandedWidth = 92.0f;
        const float previousMiscEase = m_MiscHoverT * m_MiscHoverT * (3.0f - 2.0f * m_MiscHoverT);
        const float previousMiscWidth = miscCollapsedWidth
            + previousMiscEase * (miscExpandedWidth - miscCollapsedWidth);
        ImGui::SetCursorScreenPos(miscMin);
        if (ImGui::InvisibleButton("##misc_tab", ImVec2(previousMiscWidth, initialHitMax.y - miscMin.y)))
        {
            if (m_ActiveTab != Tab::Misc)
                m_PageTransitionT = 0.0f;
            m_ActiveTab = Tab::Misc;
        }
        const bool miscHovered = ImGui::IsItemHovered();
        m_MiscHoverT += ((miscHovered ? 1.0f : 0.0f) - m_MiscHoverT)
            * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 18.0f));
        const float miscEase = m_MiscHoverT * m_MiscHoverT * (3.0f - 2.0f * m_MiscHoverT);
        const float miscWidth = miscCollapsedWidth + miscEase * (miscExpandedWidth - miscCollapsedWidth);
        const ImVec2 miscMax(miscMin.x + miscWidth, initialHitMax.y);
        if (miscEase > 0.01f)
            drawList->AddRectFilled(miscMin, miscMax,
                Color(255, 255, 255, static_cast<int>(miscEase * 20.0f)), 12.0f);
        if (m_MiscGlobe.IsValid())
        {
            const ImVec2 iconMin(std::floor(miscMin.x + 12.0f),
                std::floor((miscMin.y + miscMax.y) * 0.5f - 10.0f));
            const int alpha = m_ActiveTab == Tab::Misc ? 255 : 190 + static_cast<int>(miscEase * 45.0f);
            drawList->AddImage(reinterpret_cast<ImTextureID>(m_MiscGlobe.View()),
                iconMin, ImVec2(iconMin.x + 20.0f, iconMin.y + 20.0f),
                ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color(255, 255, 255, alpha));
        }
        if (miscEase > 0.04f)
        {
            ImVec4 textColor = colors.TextPrimary;
            textColor.w *= miscEase;
            const float h = Components::TextHeight(typography.BodyMedium);
            Components::DrawTextAt(ImVec2(miscMin.x + 40.0f, (miscMin.y + miscMax.y - h) * 0.5f),
                "Misc", typography.BodyMedium, textColor);
        }

        const ImVec2 tabMin(miscMin.x + miscWidth, miscMin.y);
        const ImVec2 tabHitMax(tabMin.x + 108.0f, initialHitMax.y);
        ImGui::SetCursorScreenPos(tabMin);
        if (ImGui::InvisibleButton("##settings_tab", ImVec2(tabHitMax.x - tabMin.x, tabHitMax.y - tabMin.y)))
        {
            if (m_ActiveTab != Tab::Settings)
                m_PageTransitionT = 0.0f;
            m_ActiveTab = Tab::Settings;
        }
        const bool tabHovered = ImGui::IsItemHovered();
        m_TabHoverT += ((tabHovered ? 1.0f : 0.0f) - m_TabHoverT)
            * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 18.0f));
        const float tabEase = m_TabHoverT * m_TabHoverT * (3.0f - 2.0f * m_TabHoverT);
        const ImVec2 tabMax(tabMin.x + 42.0f + tabEase * 66.0f, tabHitMax.y);
        if (tabEase > 0.01f)
            drawList->AddRectFilled(tabMin, tabMax,
                Color(255, 255, 255, static_cast<int>(tabEase * 20.0f)), 12.0f);
        if (m_SettingsGear.IsValid())
        {
            const ImVec2 gearMin(std::floor(tabMin.x + 12.0f),
                std::floor((tabMin.y + tabMax.y) * 0.5f - 10.0f));
            const int alpha = m_ActiveTab == Tab::Settings ? 255 : 190 + static_cast<int>(tabEase * 45.0f);
            drawList->AddImage(reinterpret_cast<ImTextureID>(m_SettingsGear.View()),
                gearMin, ImVec2(gearMin.x + 20.0f, gearMin.y + 20.0f),
                ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color(255, 255, 255, alpha));
        }
        if (tabEase > 0.04f)
        {
            const float tabTextHeight = Components::TextHeight(typography.BodyMedium);
            ImVec4 tabText = colors.TextPrimary;
            tabText.w *= tabEase;
            Components::DrawTextAt(ImVec2(tabMin.x + 40.0f, (tabMin.y + tabMax.y - tabTextHeight) * 0.5f),
                "Settings", typography.BodyMedium, tabText);
        }

        const float pageEase = 1.0f - (1.0f - m_PageTransitionT) * (1.0f - m_PageTransitionT);
        const float pageOffset = (1.0f - pageEase) * 6.0f;
        RenderOpacity::Push(pageEase);

        if (m_ActiveTab == Tab::Aimbot)
        {
            panelStyle.SurfaceColor = kRailSurface;
            Components::DrawGlassPanel(panels.LeftMin, panels.LeftMax, panelStyle);
            constexpr float buttonHeight = 42.0f;
            constexpr float buttonGap = 8.0f;
            float y = panels.LeftMin.y + 12.0f;
            const struct AimbotButton
            {
                AimbotSubtab Tab;
                Renderer::Texture* Icon;
                const char* Label;
            } buttons[] = {
                { AimbotSubtab::Global, &m_AimbotGlobal, "Global" },
                { AimbotSubtab::SilentAim, &m_AimbotSilent, "Silent Aim" },
                { AimbotSubtab::Triggerbot, &m_AimbotTrigger, "Triggerbot" }
            };

            for (const AimbotButton& button : buttons)
            {
                const ImVec2 min(panels.LeftMin.x + 8.0f, y);
                const ImVec2 max(panels.LeftMax.x - 8.0f, y + buttonHeight);
                ImGui::PushID(static_cast<int>(button.Tab));
                const ImGuiID hoverId = ImGui::GetID("##aimbot_subtab_hover");
                ImGui::SetCursorScreenPos(min);
                if (ImGui::InvisibleButton("##aimbot_subtab", ImVec2(max.x - min.x, max.y - min.y)))
                {
                    if (m_AimbotSubtab != button.Tab)
                        m_PageTransitionT = 0.0f;
                    m_AimbotSubtab = button.Tab;
                }
                const bool hovered = ImGui::IsItemHovered();
                ImGuiStorage* storage = ImGui::GetStateStorage();
                float hoverT = storage->GetFloat(hoverId, 0.0f);
                hoverT += ((hovered ? 1.0f : 0.0f) - hoverT)
                    * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 16.0f));
                storage->SetFloat(hoverId, hoverT);
                ImGui::PopID();
                const bool active = m_AimbotSubtab == button.Tab;
                if (active || hoverT > 0.01f)
                    drawList->AddRectFilled(min, max,
                        Color(255, 255, 255, active ? 18 : static_cast<int>(hoverT * 11.0f)), 11.0f);
                if (button.Icon->IsValid())
                {
                    const ImVec2 iconMin(std::floor((min.x + max.x) * 0.5f - 9.0f),
                        std::floor((min.y + max.y) * 0.5f - 9.0f));
                    drawList->AddImage(reinterpret_cast<ImTextureID>(button.Icon->View()),
                        iconMin, ImVec2(iconMin.x + 18.0f, iconMin.y + 18.0f),
                        ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                        Color(255, 255, 255, active ? 245 : 178));
                }
                y += buttonHeight + buttonGap;
            }

            if (m_AimbotSubtab == AimbotSubtab::Global)
            {
                const float pageLeft = mainMin.x + 28.0f;
                const float pageRight = panels.MainMax.x - 28.0f;
                const float pageTop = mainMin.y + 70.0f + pageOffset;
                const float gap = 20.0f;
                const float columnWidth = (pageRight - pageLeft - gap) * 0.5f;
                const ImVec2 aimbotMin(pageLeft, pageTop);
                const ImVec2 aimbotMax(pageLeft + columnWidth, pageTop + 246.0f);
                const ImVec2 tuningMin(aimbotMax.x + gap, pageTop);
                const ImVec2 tuningMax(pageRight, pageTop + 260.0f);
                const ImVec2 perWeaponMin(pageLeft, aimbotMax.y + 14.0f);
                const ImVec2 perWeaponMax(pageLeft + columnWidth, perWeaponMin.y + 84.0f);
                const ImVec2 customizationMin(tuningMin.x, tuningMax.y + 10.0f);
                const ImVec2 customizationMax(pageRight, panels.MainMax.y - 14.0f);
                DrawSectionCard(aimbotMin, aimbotMax, "Aimbot");
                DrawSectionCard(tuningMin, tuningMax, "Tuning");
                DrawSectionCard(perWeaponMin, perWeaponMax, "Per Weapon");
                DrawSectionCard(customizationMin, customizationMax, "Customization");

                constexpr float inset = 16.0f;
                const float width = columnWidth - inset * 2.0f;
                const float bindWidth = 116.0f;
                ImVec2 row(aimbotMin.x + inset, aimbotMin.y + 44.0f);
                DrawCompactToggle("aimbot_enable", row, width - bindWidth - 10.0f,
                    "Enable Aimbot", m_GlobalAimbot.Enabled);
                char primaryBuffer[32] = "None";
                const char* primaryName = m_CapturingAimbotBind == 1 ? "Press input..."
                    : (m_GlobalAimbot.PrimaryGamepadButton != 0 ? GamepadButtonName(m_GlobalAimbot.PrimaryGamepadButton)
                        : (m_GlobalAimbot.PrimaryVirtualKey != 0
                            ? VirtualKeyName(m_GlobalAimbot.PrimaryVirtualKey, primaryBuffer, sizeof(primaryBuffer)) : "None"));
                if (DrawKeybindField("primary_aimbot_bind",
                    ImVec2(aimbotMax.x - inset - bindWidth, row.y - 3.0f), bindWidth, primaryName,
                    m_CapturingAimbotBind == 1))
                {
                    m_CapturingAimbotBind = 1;
                    m_AimbotCaptureArmed = false;
                }
                row.y += 38.0f;
                Components::DrawTextAt(ImVec2(row.x, row.y + 3.0f), "or hold", typography.Small, colors.TextSecondary);
                char secondaryBuffer[32] = "None";
                const char* secondaryName = m_CapturingAimbotBind == 2 ? "Press input..."
                    : (m_GlobalAimbot.SecondaryGamepadButton != 0 ? GamepadButtonName(m_GlobalAimbot.SecondaryGamepadButton)
                        : (m_GlobalAimbot.SecondaryVirtualKey != 0
                            ? VirtualKeyName(m_GlobalAimbot.SecondaryVirtualKey, secondaryBuffer, sizeof(secondaryBuffer)) : "None"));
                if (DrawKeybindField("secondary_aimbot_bind",
                    ImVec2(aimbotMax.x - inset - bindWidth, row.y - 3.0f), bindWidth, secondaryName,
                    m_CapturingAimbotBind == 2))
                {
                    m_CapturingAimbotBind = 2;
                    m_AimbotCaptureArmed = false;
                }
                row.y += 38.0f;
                DrawCompactToggle("aimbot_fov_circle", row, width, "Draw FOV Circle", m_GlobalAimbot.DrawFovCircle); row.y += 28.0f;
                DrawCompactToggle("aimbot_visibility", row, width, "Visibility Check", m_GlobalAimbot.VisibilityCheck); row.y += 28.0f;
                DrawCompactToggle("aimbot_bone_random", row, width, "Bone Randomization", m_GlobalAimbot.BoneRandomization); row.y += 28.0f;
                DrawCompactToggle("aimbot_closest_bone", row, width, "Closest Bone Select", m_GlobalAimbot.ClosestBoneSelect);

                row = ImVec2(perWeaponMin.x + inset, perWeaponMin.y + 44.0f);
                DrawCompactToggle("aimbot_per_weapon", row, width, "Enable Per Weapon", m_GlobalAimbot.PerWeapon);

                row = ImVec2(tuningMin.x + inset, tuningMin.y + 44.0f);
                DrawCompactIntSlider("aimbot_fov", row, width, "Field of View", m_GlobalAimbot.FieldOfView, 1, 360, " deg"); row.y += 44.0f;
                DrawCompactIntSlider("aimbot_smoothness", row, width, "Smoothness", m_GlobalAimbot.Smoothness, 0, 100, "%"); row.y += 44.0f;
                DrawCompactIntSlider("aimbot_distance", row, width, "Max Distance", m_GlobalAimbot.MaxDistance, 0, 300, " m"); row.y += 46.0f;
                const char* boneOptions[] = { "Head", "Neck", "Chest", "Pelvis" };
                DrawMultiDropdown("aimbot_bones", row, width, "Target Bones", m_GlobalAimbot.Bones, boneOptions, 4); row.y += 36.0f;
                const char* priorityOptions[] = { "FOV", "Distance", "Combined" };
                int priority = static_cast<int>(m_GlobalAimbot.Priority);
                DrawDropdown("aimbot_priority", row, width, "Priority", priority, priorityOptions, 3);
                m_GlobalAimbot.Priority = static_cast<TargetPriority>(priority);

                row = ImVec2(customizationMin.x + inset, customizationMin.y + 44.0f);
                DrawCompactToggle("aimbot_disable", row, width, "Disable", m_GlobalAimbot.Disable); row.y += 30.0f;
                DrawCompactToggle("aimbot_optimize", row, width, "Optimize Aimbot", m_GlobalAimbot.OptimizeAimbot); row.y += 30.0f;
                DrawCompactToggle("aimbot_prediction", row, width, "Draw Prediction Lead", m_GlobalAimbot.PredictionLead);
            }
            else if (m_AimbotSubtab == AimbotSubtab::SilentAim)
            {
                const float pageLeft = mainMin.x + 28.0f;
                const float pageRight = panels.MainMax.x - 28.0f;
                const float pageTop = mainMin.y + 78.0f + pageOffset;
                const float gap = 20.0f;
                const float columnWidth = (pageRight - pageLeft - gap) * 0.5f;
                const ImVec2 silentMin(pageLeft, pageTop);
                const ImVec2 silentMax(pageLeft + columnWidth, pageTop + 202.0f);
                const ImVec2 tuningMin(silentMax.x + gap, pageTop);
                const ImVec2 tuningMax(pageRight, pageTop + 226.0f);
                const ImVec2 customizationMin(pageLeft, silentMax.y + 14.0f);
                const ImVec2 customizationMax(pageLeft + columnWidth, customizationMin.y + 146.0f);
                DrawSectionCard(silentMin, silentMax, "Silent Aim");
                DrawSectionCard(tuningMin, tuningMax, "Tuning");
                DrawSectionCard(customizationMin, customizationMax, "Customization");

                constexpr float inset = 16.0f;
                const float width = columnWidth - inset * 2.0f;
                ImVec2 row(silentMin.x + inset, silentMin.y + 44.0f);
                DrawCompactToggle("silent_enable", row, width, "Enable Silent Aim", m_SilentAim.Enabled); row.y += 28.0f;
                DrawCompactToggle("silent_fov", row, width, "Draw FOV", m_SilentAim.DrawFov); row.y += 28.0f;
                DrawCompactToggle("silent_visibility", row, width, "Visibility Check", m_SilentAim.VisibilityCheck); row.y += 28.0f;
                DrawCompactToggle("silent_random", row, width, "Bone Randomization", m_SilentAim.BoneRandomization); row.y += 28.0f;
                DrawCompactToggle("silent_closest", row, width, "Closest Bone Select", m_SilentAim.ClosestBoneSelect);

                row = ImVec2(tuningMin.x + inset, tuningMin.y + 44.0f);
                DrawCompactIntSlider("silent_hit_chance", row, width, "Hit Chance", m_SilentAim.HitChance, 0, 100, "%"); row.y += 46.0f;
                DrawCompactIntSlider("silent_field_of_view", row, width, "Field of View", m_SilentAim.FieldOfView, 1, 360, " deg"); row.y += 48.0f;
                const char* boneOptions[] = { "Head", "Neck", "Chest", "Pelvis" };
                DrawMultiDropdown("silent_bones", row, width, "Target Bones", m_SilentAim.Bones, boneOptions, 4); row.y += 36.0f;
                const char* priorityOptions[] = { "FOV", "Distance", "Combined" };
                int priority = static_cast<int>(m_SilentAim.Priority);
                DrawDropdown("silent_priority", row, width, "Priority", priority, priorityOptions, 3);
                m_SilentAim.Priority = static_cast<TargetPriority>(priority);

                row = ImVec2(customizationMin.x + inset, customizationMin.y + 44.0f);
                DrawCompactToggle("silent_prediction", row, width, "Prediction", m_SilentAim.Prediction); row.y += 30.0f;
                DrawCompactToggle("silent_target_line", row, width, "Show Target Line", m_SilentAim.ShowTargetLine); row.y += 30.0f;
                DrawCompactToggle("silent_optimize", row, width, "Optimize Silent Aim", m_SilentAim.Optimize);
            }
            else
            {
                const float pageLeft = mainMin.x + 28.0f;
                const float pageRight = panels.MainMax.x - 28.0f;
                const float pageTop = mainMin.y + 78.0f + pageOffset;
                const float pageWidth = pageRight - pageLeft;
                const float compactWidth = (pageWidth - 20.0f) * 0.5f;
                const float globalHeight = m_Triggerbot.AlwaysOn ? 84.0f : 174.0f;
                const ImVec2 globalMin(pageLeft, pageTop);
                const ImVec2 globalMax(pageLeft + compactWidth, pageTop + globalHeight);
                const ImVec2 weaponsMin(globalMax.x + 20.0f, pageTop);
                const ImVec2 weaponsMax(pageRight, pageTop + 212.0f);
                DrawSectionCard(globalMin, globalMax, "Triggerbot Global");
                DrawSectionCard(weaponsMin, weaponsMax, "Weapon Settings");

                constexpr float inset = 16.0f;
                const float globalControlWidth = compactWidth - inset * 2.0f;
                ImVec2 row(globalMin.x + inset, globalMin.y + 44.0f);
                if (DrawCompactToggle("trigger_always_on", row, globalControlWidth,
                    "Always On", m_Triggerbot.AlwaysOn) && m_Triggerbot.AlwaysOn)
                {
                    m_CapturingTriggerBind = 0;
                    m_TriggerCaptureArmed = false;
                }
                if (!m_Triggerbot.AlwaysOn)
                {
                    constexpr float bindWidth = 112.0f;
                    row.y += 34.0f;
                    Components::DrawTextAt(ImVec2(row.x, row.y + 3.0f), "Activation Key",
                        typography.Small, colors.TextSecondary);
                    char primaryBuffer[32] = "None";
                    const char* primaryName = m_CapturingTriggerBind == 1 ? "Press input..."
                        : (m_Triggerbot.PrimaryGamepadButton != 0 ? GamepadButtonName(m_Triggerbot.PrimaryGamepadButton)
                            : (m_Triggerbot.PrimaryVirtualKey != 0
                                ? VirtualKeyName(m_Triggerbot.PrimaryVirtualKey, primaryBuffer, sizeof(primaryBuffer)) : "None"));
                    if (DrawKeybindField("primary_trigger_bind",
                        ImVec2(globalMax.x - inset - bindWidth, row.y - 3.0f), bindWidth, primaryName,
                        m_CapturingTriggerBind == 1))
                    {
                        m_CapturingTriggerBind = 1;
                        m_TriggerCaptureArmed = false;
                    }
                    row.y += 34.0f;
                    Components::DrawTextAt(ImVec2(row.x, row.y + 3.0f), "Also hold", typography.Small, colors.TextSecondary);
                    char secondaryBuffer[32] = "None";
                    const char* secondaryName = m_CapturingTriggerBind == 2 ? "Press input..."
                        : (m_Triggerbot.SecondaryGamepadButton != 0 ? GamepadButtonName(m_Triggerbot.SecondaryGamepadButton)
                            : (m_Triggerbot.SecondaryVirtualKey != 0
                                ? VirtualKeyName(m_Triggerbot.SecondaryVirtualKey, secondaryBuffer, sizeof(secondaryBuffer)) : "None"));
                    if (DrawKeybindField("secondary_trigger_bind",
                        ImVec2(globalMax.x - inset - bindWidth, row.y - 3.0f), bindWidth, secondaryName,
                        m_CapturingTriggerBind == 2))
                    {
                        m_CapturingTriggerBind = 2;
                        m_TriggerCaptureArmed = false;
                    }
                }

                Renderer::Texture* weaponTextures[] = {
                    &m_WeaponShotgun, &m_WeaponAr, &m_WeaponSmg, &m_WeaponPistol, &m_WeaponSniper
                };
                const float width = weaponsMax.x - weaponsMin.x - inset * 2.0f;
                const ImVec2 stripMin(weaponsMin.x + inset, weaponsMin.y + 44.0f);
                const float slotGap = 6.0f;
                const float slotWidth = (width - slotGap * 4.0f) / 5.0f;
                constexpr float slotHeight = 38.0f;
                for (int index = 0; index < 5; ++index)
                {
                    const ImVec2 slotMin(stripMin.x + index * (slotWidth + slotGap), stripMin.y);
                    const ImVec2 slotMax(slotMin.x + slotWidth, slotMin.y + slotHeight);
                    ImGui::PushID(index);
                    const ImGuiID hoverId = ImGui::GetID("##weapon_hover");
                    ImGui::SetCursorScreenPos(slotMin);
                    if (ImGui::InvisibleButton("##weapon", ImVec2(slotWidth, slotHeight)))
                        m_Triggerbot.SelectedWeapon = index;
                    const bool hovered = ImGui::IsItemHovered();
                    ImGuiStorage* storage = ImGui::GetStateStorage();
                    float hoverT = storage->GetFloat(hoverId, 0.0f);
                    hoverT += ((hovered ? 1.0f : 0.0f) - hoverT)
                        * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 16.0f));
                    storage->SetFloat(hoverId, hoverT);
                    ImGui::PopID();
                    const bool selected = m_Triggerbot.SelectedWeapon == index;
                    if (selected || hoverT > 0.01f)
                        drawList->AddRectFilled(slotMin, slotMax,
                            selected ? Color(57, 67, 86, 205)
                                : Color(255, 255, 255, static_cast<int>(hoverT * 10.0f)), 11.0f);
                    if (selected)
                        drawList->AddRect(slotMin, slotMax, Color(137, 155, 194, 75), 11.0f);
                    if (weaponTextures[index]->IsValid())
                    {
                        const float imageSize = 30.0f;
                        const ImVec2 imageMin(std::floor((slotMin.x + slotMax.x - imageSize) * 0.5f),
                            std::floor((slotMin.y + slotMax.y - imageSize) * 0.5f));
                        drawList->AddImage(reinterpret_cast<ImTextureID>(weaponTextures[index]->View()),
                            imageMin, ImVec2(imageMin.x + imageSize, imageMin.y + imageSize),
                            ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                            Color(255, 255, 255, selected ? 255 : 190 + static_cast<int>(hoverT * 45.0f)));
                    }
                }

                TriggerWeaponState& weapon = m_Triggerbot.Weapons[m_Triggerbot.SelectedWeapon];
                row = ImVec2(weaponsMin.x + inset, stripMin.y + slotHeight + 9.0f);
                DrawCompactToggle("trigger_weapon_enable", row, width, "Enable for Weapon", weapon.Enabled); row.y += 32.0f;
                const float sliderGap = 14.0f;
                const float sliderWidth = (width - sliderGap) * 0.5f;
                DrawCompactIntSlider("trigger_weapon_distance", row, sliderWidth,
                    "Max Distance", weapon.MaxDistance, 1, 300, " m");
                DrawCompactIntSlider("trigger_weapon_delay", ImVec2(row.x + sliderWidth + sliderGap, row.y),
                    sliderWidth, "Click Delay", weapon.ClickDelay, 0, 500, " ms");
                row.y += 50.0f;
                const char* boneOptions[] = { "Head", "Neck", "Chest", "Pelvis", "Feet" };
                DrawMultiDropdown("trigger_weapon_bones", row, width, "Target Bones", weapon.Bones, boneOptions, 5);
            }
            RenderOpacity::Pop();
            return;
        }

        if (m_ActiveTab == Tab::Visuals)
        {
            panelStyle.SurfaceColor = kRailSurface;
            Components::DrawGlassPanel(panels.LeftMin, panels.LeftMax, panelStyle);
            constexpr float buttonHeight = 42.0f;
            constexpr float buttonGap = 8.0f;
            float y = panels.LeftMin.y + 12.0f;
            const struct VisualButton
            {
                VisualsSubtab Tab;
                Renderer::Texture* Icon;
                const char* Label;
            } buttons[] = {
                { VisualsSubtab::Body, &m_VisualsBody, "Body" },
                { VisualsSubtab::Info, &m_VisualsInfo, "Info" },
                { VisualsSubtab::Overlay, &m_VisualsOverlay, "Overlay" },
                { VisualsSubtab::Map, &m_VisualsMap, "Map" },
                { VisualsSubtab::Colors, nullptr, "Colors" }
            };

            for (const VisualButton& button : buttons)
            {
                const ImVec2 min(panels.LeftMin.x + 8.0f, y);
                const ImVec2 max(panels.LeftMax.x - 8.0f, y + buttonHeight);
                ImGui::PushID(static_cast<int>(button.Tab));
                const ImGuiID hoverId = ImGui::GetID("##visual_subtab_hover");
                ImGui::SetCursorScreenPos(min);
                if (ImGui::InvisibleButton("##visual_subtab", ImVec2(max.x - min.x, max.y - min.y)))
                {
                    if (m_VisualsSubtab != button.Tab)
                        m_PageTransitionT = 0.0f;
                    m_VisualsSubtab = button.Tab;
                }
                const bool hovered = ImGui::IsItemHovered();
                ImGuiStorage* storage = ImGui::GetStateStorage();
                float hoverT = storage->GetFloat(hoverId, 0.0f);
                hoverT += ((hovered ? 1.0f : 0.0f) - hoverT)
                    * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 16.0f));
                storage->SetFloat(hoverId, hoverT);
                ImGui::PopID();
                const bool active = m_VisualsSubtab == button.Tab;
                if (active || hoverT > 0.01f)
                    drawList->AddRectFilled(min, max,
                        Color(255, 255, 255, active ? 18 : static_cast<int>(hoverT * 11.0f)), 11.0f);

                if (button.Icon && button.Icon->IsValid())
                {
                    const ImVec2 iconMin(std::floor((min.x + max.x) * 0.5f - 9.0f),
                        std::floor((min.y + max.y) * 0.5f - 9.0f));
                    drawList->AddImage(reinterpret_cast<ImTextureID>(button.Icon->View()),
                        iconMin, ImVec2(iconMin.x + 18.0f, iconMin.y + 18.0f),
                        ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                        Color(255, 255, 255, active ? 245 : 178));
                }
                else
                {
                    const float centerX = (min.x + max.x) * 0.5f;
                    const float centerY = (min.y + max.y) * 0.5f;
                    const ImU32 swatches[4] = {
                        Color(103, 191, 255), Color(177, 121, 255), Color(255, 116, 151),
                        active ? Color(87, 224, 167) : Color(132, 136, 147)
                    };
                    for (int index = 0; index < 4; ++index)
                    {
                        const float x = centerX - 8.0f + (index % 2) * 9.0f;
                        const float sy = centerY - 8.0f + (index / 2) * 9.0f;
                        drawList->AddRectFilled(ImVec2(x, sy), ImVec2(x + 7.0f, sy + 7.0f),
                            swatches[index], 2.0f);
                    }
                }
                y += buttonHeight + buttonGap;
            }

            const float pageLeft = mainMin.x + 28.0f;
            const float pageRight = panels.MainMax.x - 28.0f;
            const float cardsTop = mainMin.y + 78.0f + pageOffset;
            const float cardsBottom = panels.MainMax.y - 28.0f;
            const float cardGap = 20.0f;
            const float columnWidth = (pageRight - pageLeft - cardGap) * 0.5f;
            const ImVec2 leftMin(pageLeft, cardsTop);
            const ImVec2 leftMax(pageLeft + columnWidth, cardsBottom);
            const ImVec2 rightMin(leftMax.x + cardGap, cardsTop);
            const ImVec2 rightMax(pageRight, cardsBottom);
            constexpr float inset = 16.0f;

            const auto beginCardContent = [](const char* id, ImVec2 min, ImVec2 max)
                {
                    ImGui::SetCursorScreenPos(ImVec2(min.x + inset, min.y + 44.0f));
                    ImGui::BeginChild(id, ImVec2(max.x - min.x - inset * 2.0f, max.y - min.y - 52.0f),
                        false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                };
            if (m_VisualsSubtab == VisualsSubtab::Body)
            {
                const ImVec2 bodyMax(leftMax.x, cardsTop + (m_BodyVisuals.Boxes ? 202.0f : 174.0f));
                const ImVec2 filterMax(rightMax.x, cardsTop + 194.0f);
                DrawSectionCard(leftMin, bodyMax, "Body ESP");
                DrawSectionCard(rightMin, filterMax, "Filtering");

                beginCardContent("##body_esp", leftMin, bodyMax);
                float width = ImGui::GetContentRegionAvail().x;
                ImVec2 row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("body_boxes", row, width, "Enable Boxes", m_BodyVisuals.Boxes);
                if (m_BodyVisuals.Boxes)
                {
                    row = ImGui::GetCursorScreenPos();
                    const char* boxOptions[] = { "Full Box", "Cornered", "3D Box" };
                    int boxType = static_cast<int>(m_BodyVisuals.BoxType);
                    DrawDropdown("box_type", row, width, "Box Type", boxType, boxOptions, 3);
                    m_BodyVisuals.BoxType = static_cast<BoxStyle>(boxType);
                }
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("body_skeleton", row, width, "Draw Skeleton", m_BodyVisuals.Skeleton);
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("body_head", row, width, "Draw Head Circle", m_BodyVisuals.HeadCircle);
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("body_reload", row, width, "Draw Reload Bar", m_BodyVisuals.ReloadBar);
                ImGui::EndChild();

                beginCardContent("##body_filter", rightMin, filterMax);
                width = ImGui::GetContentRegionAvail().x;
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("filter_bots", row, width, "Show Bots", m_BodyVisuals.ShowBots);
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("filter_visible", row, width, "Visible Only", m_BodyVisuals.VisibleOnly);
                row = ImGui::GetCursorScreenPos();
                DrawCompactToggle("filter_log", row, width, "Lobby ESP", m_BodyVisuals.LogEsp);
                row = ImGui::GetCursorScreenPos();
                DrawCompactIntSlider("filter_distance", row, width, "Max ESP Distance",
                    m_BodyVisuals.MaxDistance, 10, 300, " m");
                ImGui::EndChild();
            }
            else if (m_VisualsSubtab == VisualsSubtab::Info)
            {
                const ImVec2 playerMax(leftMax.x, cardsTop + 202.0f);
                const ImVec2 tagsMax(rightMax.x, cardsTop + 158.0f);
                DrawSectionCard(leftMin, playerMax, "Player Info");
                DrawSectionCard(rightMin, tagsMax, "Combat Tags");
                beginCardContent("##player_info", leftMin, playerMax);
                float width = ImGui::GetContentRegionAvail().x;
                const std::array<std::pair<const char*, bool*>, 5> playerRows = {{
                    { "Draw Names", &m_InfoVisuals.Names }, { "Draw Distance", &m_InfoVisuals.Distance },
                    { "Draw Platform", &m_InfoVisuals.Platform }, { "Draw Level", &m_InfoVisuals.Level },
                    { "Draw Kills", &m_InfoVisuals.Kills }
                }};
                for (const auto& [label, value] : playerRows)
                {
                    DrawCompactToggle(label, ImGui::GetCursorScreenPos(), width, label, *value);
                }
                ImGui::EndChild();

                beginCardContent("##combat_tags", rightMin, tagsMax);
                width = ImGui::GetContentRegionAvail().x;
                const std::array<std::pair<const char*, bool*>, 3> combatRows = {{
                    { "Draw Weapon", &m_InfoVisuals.Weapon }, { "Draw Ammo", &m_InfoVisuals.Ammo },
                    { "Squad Size Tag", &m_InfoVisuals.SquadSize }
                }};
                for (const auto& [label, value] : combatRows)
                {
                    DrawCompactToggle(label, ImGui::GetCursorScreenPos(), width, label, *value);
                }
                ImGui::EndChild();
            }
            else if (m_VisualsSubtab == VisualsSubtab::Overlay)
            {
                const float crosshairHeight = m_OverlayVisuals.Crosshair ? 244.0f : 96.0f;
                float overlayHeight = 174.0f;
                if (m_OverlayVisuals.NearbyCounter)
                    overlayHeight += 42.0f;
                if (m_OverlayVisuals.BulletTracers)
                    overlayHeight += 116.0f;
                overlayHeight = (std::min)(overlayHeight, cardsBottom - cardsTop);
                const ImVec2 crosshairMax(leftMax.x, cardsTop + crosshairHeight);
                const ImVec2 overlayMax(rightMax.x, cardsTop + overlayHeight);
                DrawSectionCard(leftMin, crosshairMax, "Custom Crosshair");
                DrawSectionCard(rightMin, overlayMax, "Screen Overlay");
                ImGui::SetCursorScreenPos(ImVec2(leftMin.x + inset, leftMin.y + 44.0f));
                ImGui::BeginChild("##crosshair", ImVec2(columnWidth - inset * 2.0f, crosshairMax.y - leftMin.y - 52.0f),
                    false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                float width = ImGui::GetContentRegionAvail().x;
                DrawCompactToggle("crosshair_enable", ImGui::GetCursorScreenPos(), width,
                    "Enable Crosshair", m_OverlayVisuals.Crosshair);
                if (m_OverlayVisuals.Crosshair)
                {
                    ImGui::Dummy(ImVec2(1.0f, 5.0f));
                    const char* crosshairOptions[] = { "Classic", "Dot", "Circle", "T-Shape", "Chevron" };
                    int crosshairType = static_cast<int>(m_OverlayVisuals.CrosshairType);
                    DrawDropdown("crosshair_type", ImGui::GetCursorScreenPos(), width, "Style", crosshairType,
                        crosshairOptions, 5);
                    m_OverlayVisuals.CrosshairType = static_cast<CrosshairStyle>(crosshairType);
                    ImGui::Dummy(ImVec2(1.0f, 4.0f));
                    if (DrawCompactToggle("crosshair_dynamic", ImGui::GetCursorScreenPos(), width,
                        "Dynamic Color", m_OverlayVisuals.DynamicColor) && m_OverlayVisuals.DynamicColor)
                        m_OverlayVisuals.Rainbow = false;
                    if (DrawCompactToggle("crosshair_rainbow", ImGui::GetCursorScreenPos(), width,
                        "Rainbow", m_OverlayVisuals.Rainbow) && m_OverlayVisuals.Rainbow)
                        m_OverlayVisuals.DynamicColor = false;

                    DrawCompactFloatSlider("crosshair_outline", ImGui::GetCursorScreenPos(), width,
                        "Outline", m_OverlayVisuals.OutlineThickness, 0.0f, 4.0f, "%.1f px");
                }
                ImGui::EndChild();

                ImGui::SetCursorScreenPos(ImVec2(rightMin.x + inset, rightMin.y + 44.0f));
                ImGui::BeginChild("##overlay", ImVec2(columnWidth - inset * 2.0f, overlayMax.y - rightMin.y - 52.0f),
                    false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                width = ImGui::GetContentRegionAvail().x;
                DrawCompactToggle("overlay_snaplines", ImGui::GetCursorScreenPos(), width,
                    "Draw Snaplines", m_OverlayVisuals.Snaplines);
                DrawCompactToggle("overlay_nearby", ImGui::GetCursorScreenPos(), width,
                    "Nearby Enemy Counter", m_OverlayVisuals.NearbyCounter);
                if (m_OverlayVisuals.NearbyCounter)
                {
                    DrawCompactIntSlider("overlay_nearby_distance", ImGui::GetCursorScreenPos(), width,
                        "Nearby Distance", m_OverlayVisuals.NearbyDistance, 0, 200, " m");
                }
                DrawCompactToggle("overlay_spectators", ImGui::GetCursorScreenPos(), width,
                    "Spectator List", m_OverlayVisuals.SpectatorList);
                DrawCompactToggle("overlay_tracers", ImGui::GetCursorScreenPos(), width,
                    "Bullet Tracers", m_OverlayVisuals.BulletTracers);
                if (m_OverlayVisuals.BulletTracers)
                {
                    DrawCompactToggle("overlay_rainbow_tracers", ImGui::GetCursorScreenPos(), width,
                        "Rainbow Tracers", m_OverlayVisuals.RainbowTracers);
                    DrawCompactFloatSlider("overlay_tracer_thickness", ImGui::GetCursorScreenPos(), width,
                        "Tracer Thickness", m_OverlayVisuals.TracerThickness, 0.5f, 5.0f, "%.1f px");
                    DrawCompactFloatSlider("overlay_tracer_lifetime", ImGui::GetCursorScreenPos(), width,
                        "Tracer Lifetime", m_OverlayVisuals.TracerLifetime, 0.5f, 10.0f, "%.1f s");
                }
                ImGui::EndChild();
            }
            else if (m_VisualsSubtab == VisualsSubtab::Map)
            {
                const ImVec2 radarMax(leftMax.x, cardsTop + (m_MapVisuals.Radar ? 230.0f : 184.0f));
                DrawSectionCard(leftMin, radarMax, "Radar");
                beginCardContent("##radar", leftMin, radarMax);
                const float width = ImGui::GetContentRegionAvail().x;
                DrawCompactToggle("radar_enable", ImGui::GetCursorScreenPos(), width, "Enable Radar", m_MapVisuals.Radar);
                DrawCompactToggle("radar_names", ImGui::GetCursorScreenPos(), width, "Show Names", m_MapVisuals.Names);
                DrawCompactToggle("radar_bots", ImGui::GetCursorScreenPos(), width, "Show Bots", m_MapVisuals.Bots);
                DrawCompactToggle("radar_teammates", ImGui::GetCursorScreenPos(), width, "Show Teammates", m_MapVisuals.Teammates);
                if (m_MapVisuals.Radar)
                {
                    DrawCompactIntSlider("radar_range", ImGui::GetCursorScreenPos(), width, "Radar Range",
                        m_MapVisuals.Range, 10, 300, " m");
                }
                ImGui::EndChild();
            }
            else
            {
                ImGui::SetCursorScreenPos(ImVec2(pageLeft, cardsTop));
                ImGui::BeginChild("##colors_page", ImVec2(pageRight - pageLeft, cardsBottom - cardsTop),
                    false, ImGuiWindowFlags_NoBackground);
                const float localWidth = ImGui::GetContentRegionAvail().x - 8.0f;
                const float localColumn = (localWidth - cardGap) * 0.5f;
                const ImVec2 base = ImGui::GetCursorScreenPos();
                const ImVec2 scaleMin = base;
                const ImVec2 scaleMax(base.x + localColumn, base.y + 188.0f);
                const ImVec2 combatMin(base.x + localColumn + cardGap, base.y);
                const ImVec2 combatMax(base.x + localWidth, base.y + 188.0f);
                const ImVec2 playerMin(base.x, scaleMax.y + 14.0f);
                const ImVec2 playerMax(base.x + localColumn, playerMin.y + 188.0f);
                const ImVec2 globalMin(combatMin.x, combatMax.y + 14.0f);
                const ImVec2 globalMax(base.x + localWidth, globalMin.y + 188.0f);
                DrawSectionCard(scaleMin, scaleMax, "ESP Scale");
                DrawSectionCard(combatMin, combatMax, "Combat Colors");
                DrawSectionCard(playerMin, playerMax, "Player ESP Colors");
                DrawSectionCard(globalMin, globalMax, "Global Color");

                float width = localColumn - inset * 2.0f;
                ImVec2 row(scaleMin.x + inset, scaleMin.y + 46.0f);
                DrawCompactFloatSlider("color_box_thickness", row, width, "Box Thickness",
                    m_ColorsVisuals.BoxThickness, 0.5f, 5.0f, "%.1f px");
                row.y += 46.0f;
                DrawCompactFloatSlider("color_text_scale", row, width, "Text Scale",
                    m_ColorsVisuals.TextScale, 0.5f, 2.0f, "%.2f x");
                row.y += 46.0f;
                DrawCompactFloatSlider("color_distance_scale", row, width, "Distance Scaling",
                    m_ColorsVisuals.DistanceScaling, 0.0f, 1.0f, "%.2f");

                row = ImVec2(combatMin.x + inset, combatMin.y + 46.0f);
                const float colorGap = 12.0f;
                const float colorWidth = (width - colorGap) * 0.5f;
                DrawColorRow("color_level", row, colorWidth, "Level", m_ColorsVisuals.Level);
                DrawColorRow("color_kills", ImVec2(row.x + colorWidth + colorGap, row.y),
                    colorWidth, "Kills", m_ColorsVisuals.Kills); row.y += 30.0f;
                DrawColorRow("color_weapon", row, colorWidth, "Weapon", m_ColorsVisuals.Weapon);
                DrawColorRow("color_ammo", ImVec2(row.x + colorWidth + colorGap, row.y),
                    colorWidth, "Ammo", m_ColorsVisuals.Ammo); row.y += 30.0f;
                DrawColorRow("color_tracers", row, width, "Bullet Tracers", m_ColorsVisuals.BulletTracers); row.y += 34.0f;
                DrawCompactFloatSlider("color_tracer_alpha", row, width, "Tracer Alpha",
                    m_ColorsVisuals.TracerAlpha, 0.0f, 1.0f, "%.2f");

                row = ImVec2(playerMin.x + inset, playerMin.y + 46.0f);
                DrawColorRow("color_reload", row, width, "Reload Bar", m_ColorsVisuals.ReloadBar); row.y += 29.0f;
                DrawColorRow("color_player_name", row, width, "Player Name", m_ColorsVisuals.PlayerName); row.y += 29.0f;
                DrawColorRow("color_distance", row, width, "Distance", m_ColorsVisuals.Distance); row.y += 29.0f;
                DrawColorRow("color_platform", row, width, "Platform", m_ColorsVisuals.Platform);

                row = ImVec2(globalMin.x + inset, globalMin.y + 46.0f);
                DrawCompactToggle("visibility_colors", row, width, "Visible / Hidden", m_ColorsVisuals.VisibilityColors);
                row.y += 31.0f;
                if (m_ColorsVisuals.VisibilityColors)
                {
                    DrawColorRow("color_visible", row, width, "Visible", m_ColorsVisuals.Visible); row.y += 29.0f;
                    DrawColorRow("color_hidden", row, width, "Hidden", m_ColorsVisuals.Hidden);
                }
                else
                {
                    DrawColorRow("color_global", row, width, "Global", m_ColorsVisuals.Global);
                }
                ImGui::SetCursorScreenPos(ImVec2(base.x, globalMax.y + 2.0f));
                ImGui::Dummy(ImVec2(1.0f, 1.0f));
                ImGui::EndChild();
            }
            RenderOpacity::Pop();
            return;
        }

        if (m_ActiveTab == Tab::Misc)
        {
            panelStyle.SurfaceColor = kRailSurface;
            Components::DrawGlassPanel(panels.LeftMin, panels.LeftMax, panelStyle);
            const ImVec2 subtabMin(panels.LeftMin.x + 8.0f, panels.LeftMin.y + 12.0f);
            const ImVec2 subtabMax(panels.LeftMax.x - 8.0f, subtabMin.y + 42.0f);
            drawList->AddRectFilled(subtabMin, subtabMax, Color(255, 255, 255, 15), 11.0f);
            if (m_ExploitsBomb.IsValid())
            {
                const ImVec2 iconMin(std::floor((subtabMin.x + subtabMax.x) * 0.5f - 9.0f),
                    std::floor((subtabMin.y + subtabMax.y) * 0.5f - 9.0f));
                drawList->AddImage(reinterpret_cast<ImTextureID>(m_ExploitsBomb.View()),
                    iconMin, ImVec2(iconMin.x + 18.0f, iconMin.y + 18.0f),
                    ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color(255, 255, 255, 220));
            }

            const float pageLeft = mainMin.x + 28.0f;
            const float pageRight = panels.MainMax.x - 28.0f;
            const float pageTop = mainMin.y + 84.0f + pageOffset;
            const float gap = 20.0f;
            const float columnWidth = (pageRight - pageLeft - gap) * 0.5f;
            const float leftX = pageLeft;
            const float rightX = pageLeft + columnWidth + gap;
            const float cardTop = pageTop;
            const float leftCardHeight = 248.0f + (m_FovChanger ? 29.0f : 0.0f)
                + (m_FovZoom ? 29.0f : 0.0f);
            const float rightCardHeight = 184.0f + (m_PlayerSize ? 30.0f : 0.0f);
            DrawSectionCard(ImVec2(leftX, cardTop),
                ImVec2(leftX + columnWidth, cardTop + leftCardHeight), "General exploits");
            DrawSectionCard(ImVec2(rightX, cardTop),
                ImVec2(rightX + columnWidth, cardTop + rightCardHeight), "Combat");

            const float inset = 14.0f;
            const float controlWidth = columnWidth - inset * 2.0f;
            float y = cardTop + 44.0f;
            DrawLabel(ImVec2(leftX + inset, y), "FOV changer", "Override the default field of view.");
            DrawToggle("fov_changer", ImVec2(leftX + columnWidth - inset - 38.0f, y + 4.0f),
                m_FovChanger, m_FovChangerT);
            y += 42.0f;
            if (m_FovChanger)
            {
                char value[12]; snprintf(value, sizeof(value), "%d", m_CustomFov);
                constexpr float valueRail = 38.0f;
                const float sliderWidth = controlWidth * 0.66f;
                DrawIntRangeSlider("custom_fov", ImVec2(leftX + inset, y),
                    sliderWidth, m_CustomFov, 60, 150);
                Components::DrawTextCentered(ImVec2(leftX + inset + sliderWidth + 6.0f, y + 5.0f),
                    valueRail, value, typography.Small, colors.TextPrimary);
                y += 29.0f;
            }

            DrawLabel(ImVec2(leftX + inset, y), "FOV zoom", "Hold a bound key to zoom.");
            DrawToggle("fov_zoom", ImVec2(leftX + columnWidth - inset - 38.0f, y + 4.0f),
                m_FovZoom, m_FovZoomT);
            y += 42.0f;
            if (m_FovZoom)
            {
                char zoomValue[12]; snprintf(zoomValue, sizeof(zoomValue), "%d", m_ZoomFov);
                constexpr float valueRail = 30.0f;
                const float sliderWidth = controlWidth * 0.50f;
                DrawIntRangeSlider("zoom_fov", ImVec2(leftX + inset, y),
                    sliderWidth, m_ZoomFov, 10, 90);
                Components::DrawTextCentered(ImVec2(leftX + inset + sliderWidth + 6.0f, y + 5.0f),
                    valueRail, zoomValue, typography.Small, colors.TextPrimary);

                char zoomKeyBuffer[32] = "None";
                const char* zoomKeyName = m_CapturingZoomKey ? "Press input..."
                    : (m_ZoomGamepadButton != 0 ? GamepadButtonName(m_ZoomGamepadButton)
                        : (m_ZoomVirtualKey != 0 ? VirtualKeyName(m_ZoomVirtualKey, zoomKeyBuffer, sizeof(zoomKeyBuffer)) : "None"));
                const float availableKeyWidth = controlWidth - sliderWidth - valueRail - 12.0f;
                const float zoomKeyWidth = (std::max)(48.0f,
                    (std::min)(availableKeyWidth, Components::TextWidth(zoomKeyName, typography.Small) + 16.0f));
                const ImVec2 zoomKeyMin(leftX + inset + sliderWidth + valueRail + 12.0f, y + 1.0f);
                ImGui::SetCursorScreenPos(zoomKeyMin);
                if (ImGui::InvisibleButton("##zoom_key", ImVec2(zoomKeyWidth, 24.0f)))
                {
                    m_CapturingZoomKey = true;
                    m_ZoomCaptureArmed = false;
                }
                drawList->AddRectFilled(zoomKeyMin, ImVec2(zoomKeyMin.x + zoomKeyWidth, zoomKeyMin.y + 24.0f),
                    Color(24, 27, 35), 7.0f);
                const float zoomKeyTextHeight = Components::TextHeight(typography.Small);
                Components::DrawTextCentered(ImVec2(zoomKeyMin.x,
                    zoomKeyMin.y + (24.0f - zoomKeyTextHeight) * 0.5f), zoomKeyWidth,
                    zoomKeyName, typography.Small, m_CapturingZoomKey ? colors.TextPrimary : colors.TextSecondary);
                y += 29.0f;
            }
            DrawLabel(ImVec2(leftX + inset, y), "Force T-pose", "Force the local player pose.");
            DrawToggle("tpose", ImVec2(leftX + columnWidth - inset - 38.0f, y + 4.0f),
                m_ForceTPose, m_ForceTPoseT);
            y += 52.0f;
            DrawLabel(ImVec2(leftX + inset, y), "Instant interaction", "Remove interaction hold time.");
            DrawToggle("instant", ImVec2(leftX + columnWidth - inset - 38.0f, y + 4.0f),
                m_InstantInteraction, m_InstantInteractionT);

            y = cardTop + 44.0f;
            DrawLabel(ImVec2(rightX + inset, y), "Spinbot", "Continuously rotate the player.");
            DrawToggle("spinbot", ImVec2(rightX + columnWidth - inset - 38.0f, y + 4.0f),
                m_Spinbot, m_SpinbotT);
            y += 52.0f;
            DrawLabel(ImVec2(rightX + inset, y), "Player size", "Apply a custom player scale.");
            DrawToggle("player_size", ImVec2(rightX + columnWidth - inset - 38.0f, y + 4.0f),
                m_PlayerSize, m_PlayerSizeT);
            y += 42.0f;
            if (m_PlayerSize)
            {
                char scale[16]; snprintf(scale, sizeof(scale), "%.1f", m_SizeScale);
                constexpr float valueRail = 42.0f;
                const float sliderWidth = controlWidth * 0.66f;
                DrawFloatRangeSlider("size_scale", ImVec2(rightX + inset, y),
                    sliderWidth, m_SizeScale, 0.5f, 5.0f);
                Components::DrawTextCentered(ImVec2(rightX + inset + sliderWidth + 6.0f, y + 5.0f),
                    valueRail, scale, typography.Small, colors.TextPrimary);
                y += 30.0f;
            }
            DrawLabel(ImVec2(rightX + inset, y), "Tiny player", "Use the smallest player preset.");
            DrawToggle("tiny_player", ImVec2(rightX + columnWidth - inset - 38.0f, y + 4.0f),
                m_TinyPlayer, m_TinyPlayerT);
            RenderOpacity::Pop();
            return;
        }

        const float contentLeft = mainMin.x + 28.0f;
        const float contentRight = panels.MainMax.x - 28.0f;
        const float bodyY = mainMin.y + 92.0f + pageOffset;
        const float statusWidth = 228.0f;
        const float columnGap = 30.0f;
        const float settingsWidth = contentRight - contentLeft - statusWidth - columnGap;

        float rowY = bodyY;
        const float groupTop = rowY - 12.0f;
        const float groupBottom = rowY + (m_EnableFpsCap ? 280.0f : 244.0f);
        const float rowLeft = contentLeft + 12.0f;
        const float rowRight = contentLeft + settingsWidth - 12.0f;
        drawList->AddRectFilled(ImVec2(contentLeft, groupTop),
            ImVec2(contentLeft + settingsWidth, groupBottom), Color(17, 20, 27), 12.0f);
        drawList->AddRect(ImVec2(contentLeft, groupTop),
            ImVec2(contentLeft + settingsWidth, groupBottom), Color(255, 255, 255, 11), 12.0f);

        DrawLabel(ImVec2(rowLeft, rowY), "Menu key", "Keyboard, mouse, or controller button.");
        char virtualKeyName[32] = "Key";
        const char* keyName = m_CapturingMenuKey ? "Press input..."
            : (m_MenuGamepadButton != 0 ? GamepadButtonName(m_MenuGamepadButton)
                : VirtualKeyName(m_MenuVirtualKey, virtualKeyName, sizeof(virtualKeyName)));
        const float keyWidth = (std::max)(62.0f, Components::TextWidth(keyName, typography.Small) + 26.0f);
        const ImVec2 keyMin(rowRight - keyWidth, rowY + 1.0f);
        const ImVec2 keySize(keyWidth, 30.0f);
        ImGui::SetCursorScreenPos(keyMin);
        if (ImGui::InvisibleButton("##menu_key", keySize))
        {
            m_CapturingMenuKey = true;
            m_CaptureArmed = false;
        }
        const bool keyHovered = ImGui::IsItemHovered();
        drawList->AddRectFilled(keyMin, ImVec2(keyMin.x + keySize.x, keyMin.y + keySize.y),
            keyHovered || m_CapturingMenuKey ? Color(31, 34, 43) : Color(24, 27, 35), 9.0f);
        drawList->AddRect(keyMin, ImVec2(keyMin.x + keySize.x, keyMin.y + keySize.y),
            m_CapturingMenuKey ? Color(130, 135, 148) : Color(255, 255, 255, 18), 9.0f);
        const float keyTextHeight = Components::TextHeight(typography.Small);
        Components::DrawTextCentered(ImVec2(keyMin.x, keyMin.y + (keySize.y - keyTextHeight) * 0.5f), keySize.x,
            keyName, typography.Small, m_CapturingMenuKey ? colors.TextPrimary : colors.TextSecondary);
        rowY += 64.0f;

        DrawLabel(ImVec2(rowLeft, rowY), "Stream-proof mode", "Exclude the interface from supported captures.");
        DrawToggle("stream_proof", ImVec2(rowRight - 38.0f, rowY + 4.0f),
            m_StreamProof, m_StreamProofT);
        rowY += 64.0f;

        DrawLabel(ImVec2(rowLeft, rowY), "Show watermark", "Display a compact session indicator in-game.");
        DrawToggle("watermark", ImVec2(rowRight - 38.0f, rowY + 4.0f),
            m_ShowWatermark, m_WatermarkT);
        rowY += 64.0f;

        DrawLabel(ImVec2(rowLeft, rowY), "Enable FPS cap", "Limit the maximum rendered frame rate.");
        DrawToggle("fps_cap", ImVec2(rowRight - 38.0f, rowY + 4.0f),
            m_EnableFpsCap, m_FpsCapT);
        if (m_EnableFpsCap)
        {
            const ImVec2 sliderMin(rowLeft, rowY + 38.0f);
            constexpr float valueRail = 46.0f;
            const float sliderWidth = (rowRight - rowLeft) * 0.72f;
            DrawFpsSlider("fps_slider", sliderMin, sliderWidth, m_FpsCap);
            char fpsText[16];
            snprintf(fpsText, sizeof(fpsText), "%d", m_FpsCap);
            Components::DrawTextCentered(ImVec2(rowLeft + sliderWidth + 6.0f, sliderMin.y + 5.0f),
                valueRail, fpsText, typography.Small, colors.TextPrimary);
        }

        const float statusX = contentLeft + settingsWidth + columnGap;
        const ImVec2 statusMin(statusX, groupTop);
        const ImVec2 statusMax(statusX + statusWidth, statusMin.y + 196.0f);
        drawList->AddRectFilled(statusMin, statusMax, Color(17, 20, 27), 14.0f);
        drawList->AddRect(statusMin, statusMax, Color(255, 255, 255, 13), 14.0f);
        Components::DrawTextAt(ImVec2(statusMin.x + 18.0f, statusMin.y + 12.0f),
            "System status", typography.BodyMedium, colors.TextPrimary);
        Components::DrawTextAt(ImVec2(statusMin.x + 18.0f, statusMin.y + 39.0f),
            "All core services available", typography.Small, colors.TextSecondary);
        DrawStatusRow(ImVec2(statusMin.x + 18.0f, statusMin.y + 76.0f), statusWidth - 36.0f,
            "Driver", "Active", true);
        DrawStatusRow(ImVec2(statusMin.x + 18.0f, statusMin.y + 112.0f), statusWidth - 36.0f,
            "Game", "Not running", false);
        DrawStatusRow(ImVec2(statusMin.x + 18.0f, statusMin.y + 148.0f), statusWidth - 36.0f,
            "License", "29d 14h", true);
        RenderOpacity::Pop();
    }
}
