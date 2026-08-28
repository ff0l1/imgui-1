#pragma once

#include "imgui.h"

struct ImFont;

namespace UI
{
    struct ThemeColors
    {
        ImVec4 Background;
        ImVec4 InputSurface;
        ImVec4 InputSurfaceHover;
        ImVec4 InputSurfaceFocus;
        ImVec4 TextPrimary;
        ImVec4 TextSecondary;
        ImVec4 TextPlaceholder;
        ImVec4 TextDisabled;
        ImVec4 Accent;
        ImVec4 AccentHover;
        ImVec4 AccentPressed;
        ImVec4 AccentText;
        ImVec4 AccentDisabled;
        ImVec4 Border;
        ImVec4 BorderFocus;
        ImVec4 ControlHover;
        ImVec4 CloseHover;
        ImVec4 SnowParticle;
        ImVec4 CardSurface;
        ImVec4 CardSurfaceHover;
        ImVec4 CardBorder;
        ImVec4 OutlineSurface;
        ImVec4 OutlineSurfaceHover;
        ImVec4 OutlineBorder;
        ImVec4 SidebarActive;
        ImVec4 Divider;
        ImVec4 Danger;
        ImVec4 GlassSurface;
        ImVec4 GlassSheen;
        ImVec4 GlassBorder;
        ImVec4 Shadow;
    };

    struct ThemeMetrics
    {
        float ContentWidth;
        float InputHeight;
        float InputRadius;
        float InputPaddingX;
        float ButtonHeight;
        float ButtonRadius;
        float IconButtonSize;
        float WindowControlSize;
        float TitleBarHeight;
    };

    struct ThemeTypography
    {
        ImFont* Heading = nullptr;
        ImFont* Body = nullptr;
        ImFont* BodyMedium = nullptr;
        ImFont* Small = nullptr;
        ImFont* IconSymbol = nullptr;
        ImFont* IconAction = nullptr;
        ImFont* IconControl = nullptr;
        ImFont* IconBrand = nullptr;
        ImFont* IconAwesome = nullptr;
    };

    struct ThemeAnimation
    {
        float HoverSeconds;
        float FocusSeconds;
        float PressSeconds;
        float TransitionSeconds;
        float ScreenOutSeconds;
        float ScreenInSeconds;
    };

    struct ThemeSnow
    {
        int ParticleCount;
        float MinSize;
        float MaxSize;
        float MinFallSpeed;
        float MaxFallSpeed;
        float MinOpacity;
        float MaxOpacity;
        float DriftAmount;
        float DriftSpeed;
        float BlurredFraction;
    };

    class Theme
    {
    public:
        static Theme& Get();

        const ThemeColors& Colors() const { return m_Colors; }
        const ThemeMetrics& Metrics() const { return m_Metrics; }
        const ThemeTypography& Typography() const { return m_Typography; }
        const ThemeAnimation& Animation() const { return m_Animation; }
        const ThemeSnow& Snow() const { return m_Snow; }

        ThemeTypography& MutableTypography() { return m_Typography; }

    private:
        Theme();

        ThemeColors m_Colors;
        ThemeMetrics m_Metrics;
        ThemeTypography m_Typography;
        ThemeAnimation m_Animation;
        ThemeSnow m_Snow;
    };
}
