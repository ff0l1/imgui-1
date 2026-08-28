#pragma once

#include "Screen.h"
#include "Renderer/Texture.h"
#include <Windows.h>
#include <array>

namespace UI::Screens
{
    class MainMenuScreen : public Screen
    {
    public:
        struct PanelRects
        {
            ImVec2 TopMin;
            ImVec2 TopMax;
            ImVec2 LeftMin;
            ImVec2 LeftMax;
            ImVec2 MainMin;
            ImVec2 MainMax;
        };

        static PanelRects GetPanelRects(ImVec2 regionMin, ImVec2 regionSize);
        static float PanelRadius();
        bool HasLeftPanel() const { return m_ActiveTab != Tab::Settings; }
        void Initialize(ID3D11Device* device) override;
        void Update(float deltaSeconds) override;
        void Draw(ImVec2 regionMin, ImVec2 regionSize) override;
        bool ConsumeToggleRequest();
        bool IsFpsCapEnabled() const { return m_EnableFpsCap; }
        int FpsCap() const { return m_FpsCap; }
        int MenuVirtualKey() const { return m_MenuVirtualKey; }
        bool UsesNativeKeyboardHotkey() const;

    private:
        enum class Tab { Settings, Misc, Visuals, Aimbot };
        enum class VisualsSubtab { Body, Info, Overlay, Map, Colors };
        enum class AimbotSubtab { Global, SilentAim, Triggerbot };
        enum class BoxStyle { Full, Cornered, ThreeDimensional };
        enum class CrosshairStyle { Classic, Dot, Circle, TShape, Chevron };
        enum class TargetPriority { Fov, Distance, Combined };

        struct GlobalAimbotState
        {
            bool Enabled = false;
            int PrimaryVirtualKey = 0;
            WORD PrimaryGamepadButton = 0;
            int SecondaryVirtualKey = 0;
            WORD SecondaryGamepadButton = 0;
            bool DrawFovCircle = false;
            bool VisibilityCheck = false;
            bool BoneRandomization = false;
            bool ClosestBoneSelect = false;
            int FieldOfView = 90;
            int Smoothness = 50;
            int MaxDistance = 150;
            unsigned int Bones = 1u;
            TargetPriority Priority = TargetPriority::Fov;
            bool PredictionLead = false;
            bool Disable = false;
            bool OptimizeAimbot = false;
            bool PerWeapon = false;
        };

        struct SilentAimState
        {
            bool Enabled = false;
            bool DrawFov = false;
            bool VisibilityCheck = false;
            bool BoneRandomization = false;
            bool ClosestBoneSelect = false;
            int HitChance = 75;
            int FieldOfView = 90;
            unsigned int Bones = 1u;
            TargetPriority Priority = TargetPriority::Fov;
            bool Prediction = false;
            bool ShowTargetLine = false;
            bool Optimize = false;
        };

        struct TriggerWeaponState
        {
            bool Enabled = false;
            int MaxDistance = 150;
            int ClickDelay = 50;
            unsigned int Bones = 1u;
        };

        struct TriggerbotState
        {
            bool AlwaysOn = false;
            int PrimaryVirtualKey = 0;
            WORD PrimaryGamepadButton = 0;
            int SecondaryVirtualKey = 0;
            WORD SecondaryGamepadButton = 0;
            int SelectedWeapon = 0;
            std::array<TriggerWeaponState, 5> Weapons{};
        };

        struct BodyVisualsState
        {
            bool Boxes = false;
            BoxStyle BoxType = BoxStyle::Full;
            bool Skeleton = false;
            bool HeadCircle = false;
            bool ReloadBar = false;
            bool ShowBots = false;
            bool VisibleOnly = false;
            bool LogEsp = false;
            int MaxDistance = 150;
        };

        struct InfoVisualsState
        {
            bool Names = false;
            bool Distance = false;
            bool Platform = false;
            bool Level = false;
            bool Kills = false;
            bool Weapon = false;
            bool Ammo = false;
            bool SquadSize = false;
        };

        struct OverlayVisualsState
        {
            bool Crosshair = false;
            CrosshairStyle CrosshairType = CrosshairStyle::Classic;
            bool DynamicColor = false;
            bool Rainbow = false;
            float CrosshairThickness = 2.0f;
            float CrosshairGap = 5.0f;
            float CrosshairLength = 8.0f;
            float CrosshairSize = 8.0f;
            float CrosshairRotation = 0.0f;
            float OutlineThickness = 1.0f;
            bool Snaplines = false;
            bool NearbyCounter = false;
            int NearbyDistance = 100;
            bool SpectatorList = false;
            bool BulletTracers = false;
            bool RainbowTracers = false;
            float TracerThickness = 1.5f;
            float TracerLifetime = 3.0f;
        };

        struct MapVisualsState
        {
            bool Radar = false;
            bool Names = false;
            bool Bots = false;
            bool Teammates = false;
            int Range = 100;
        };

        struct ColorsVisualsState
        {
            float BoxThickness = 1.5f;
            float TextScale = 1.0f;
            float DistanceScaling = 0.5f;
            ImVec4 Level{ 0.72f, 0.63f, 1.0f, 1.0f };
            ImVec4 Kills{ 1.0f, 0.48f, 0.46f, 1.0f };
            ImVec4 Weapon{ 0.95f, 0.78f, 0.38f, 1.0f };
            ImVec4 Ammo{ 0.45f, 0.76f, 1.0f, 1.0f };
            ImVec4 BulletTracers{ 1.0f, 0.58f, 0.30f, 1.0f };
            float TracerAlpha = 0.85f;
            ImVec4 ReloadBar{ 0.35f, 0.82f, 0.64f, 1.0f };
            ImVec4 PlayerName{ 0.96f, 0.96f, 0.98f, 1.0f };
            ImVec4 Distance{ 0.70f, 0.73f, 0.80f, 1.0f };
            ImVec4 Platform{ 0.54f, 0.72f, 1.0f, 1.0f };
            bool VisibilityColors = false;
            ImVec4 Global{ 0.94f, 0.94f, 0.97f, 1.0f };
            ImVec4 Visible{ 0.37f, 0.90f, 0.58f, 1.0f };
            ImVec4 Hidden{ 1.0f, 0.39f, 0.38f, 1.0f };
        };

        Renderer::Texture m_SettingsGear;
        Renderer::Texture m_MiscGlobe;
        Renderer::Texture m_ExploitsBomb;
        Renderer::Texture m_VisualsEye;
        Renderer::Texture m_VisualsBody;
        Renderer::Texture m_VisualsInfo;
        Renderer::Texture m_VisualsOverlay;
        Renderer::Texture m_VisualsMap;
        Renderer::Texture m_AimbotCrosshairs;
        Renderer::Texture m_AimbotGlobal;
        Renderer::Texture m_AimbotSilent;
        Renderer::Texture m_AimbotTrigger;
        Renderer::Texture m_WeaponShotgun;
        Renderer::Texture m_WeaponAr;
        Renderer::Texture m_WeaponSmg;
        Renderer::Texture m_WeaponPistol;
        Renderer::Texture m_WeaponSniper;
        Tab m_ActiveTab = Tab::Settings;
        VisualsSubtab m_VisualsSubtab = VisualsSubtab::Body;
        AimbotSubtab m_AimbotSubtab = AimbotSubtab::Global;
        GlobalAimbotState m_GlobalAimbot;
        SilentAimState m_SilentAim;
        TriggerbotState m_Triggerbot;
        int m_CapturingAimbotBind = 0;
        bool m_AimbotCaptureArmed = false;
        int m_CapturingTriggerBind = 0;
        bool m_TriggerCaptureArmed = false;
        BodyVisualsState m_BodyVisuals;
        InfoVisualsState m_InfoVisuals;
        OverlayVisualsState m_OverlayVisuals;
        MapVisualsState m_MapVisuals;
        ColorsVisualsState m_ColorsVisuals;
        bool m_CapturingMenuKey = false;
        bool m_CaptureArmed = false;
        int m_MenuVirtualKey = VK_INSERT;
        WORD m_MenuGamepadButton = 0;
        bool m_BindingWasDown = false;
        bool m_ToggleRequested = false;
        bool m_StreamProof = false;
        bool m_ShowWatermark = true;
        bool m_EnableFpsCap = false;
        int m_FpsCap = 240;
        float m_TabHoverT = 0.0f;
        float m_MiscHoverT = 0.0f;
        float m_VisualsHoverT = 0.0f;
        float m_AimbotHoverT = 0.0f;
        float m_StreamProofT = 0.0f;
        float m_WatermarkT = 1.0f;
        float m_FpsCapT = 0.0f;
        bool m_FovChanger = false;
        bool m_FovZoom = false;
        int m_CustomFov = 90;
        int m_ZoomFov = 45;
        int m_ZoomVirtualKey = 0;
        WORD m_ZoomGamepadButton = 0;
        bool m_CapturingZoomKey = false;
        bool m_ZoomCaptureArmed = false;
        bool m_ForceTPose = false;
        bool m_InstantInteraction = false;
        bool m_Spinbot = false;
        bool m_PlayerSize = false;
        float m_SizeScale = 1.0f;
        bool m_TinyPlayer = false;
        float m_FovChangerT = 0.0f;
        float m_FovZoomT = 0.0f;
        float m_ForceTPoseT = 0.0f;
        float m_InstantInteractionT = 0.0f;
        float m_SpinbotT = 0.0f;
        float m_PlayerSizeT = 0.0f;
        float m_TinyPlayerT = 0.0f;
        float m_PageTransitionT = 1.0f;
    };
}
