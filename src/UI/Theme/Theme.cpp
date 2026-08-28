#include "Theme.h"

namespace UI
{
    static ImVec4 RGBA(int r, int g, int b, float a)
    {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a);
    }

    Theme::Theme()
    {
        m_Colors.Background = RGBA(11, 11, 13, 1.0f);
        m_Colors.InputSurface = RGBA(255, 255, 255, 0.040f);
        m_Colors.InputSurfaceHover = RGBA(255, 255, 255, 0.060f);
        m_Colors.InputSurfaceFocus = RGBA(255, 255, 255, 0.078f);
        m_Colors.TextPrimary = RGBA(242, 242, 245, 1.0f);
        m_Colors.TextSecondary = RGBA(124, 124, 134, 1.0f);
        m_Colors.TextPlaceholder = RGBA(92, 92, 101, 1.0f);
        m_Colors.TextDisabled = RGBA(88, 88, 95, 1.0f);
        m_Colors.Accent = RGBA(240, 240, 243, 1.0f);
        m_Colors.AccentHover = RGBA(255, 255, 255, 1.0f);
        m_Colors.AccentPressed = RGBA(208, 208, 213, 1.0f);
        m_Colors.AccentText = RGBA(12, 12, 14, 1.0f);
        m_Colors.AccentDisabled = RGBA(255, 255, 255, 0.085f);
        m_Colors.Border = RGBA(255, 255, 255, 0.055f);
        m_Colors.BorderFocus = RGBA(255, 255, 255, 0.185f);
        m_Colors.ControlHover = RGBA(255, 255, 255, 0.085f);
        m_Colors.CloseHover = RGBA(232, 78, 71, 0.90f);
        m_Colors.SnowParticle = RGBA(236, 240, 248, 1.0f);
        m_Colors.CardSurface = RGBA(255, 255, 255, 0.038f);
        m_Colors.CardSurfaceHover = RGBA(255, 255, 255, 0.058f);
        m_Colors.CardBorder = RGBA(255, 255, 255, 0.055f);
        m_Colors.OutlineSurface = RGBA(255, 255, 255, 0.030f);
        m_Colors.OutlineSurfaceHover = RGBA(255, 255, 255, 0.070f);
        m_Colors.OutlineBorder = RGBA(255, 255, 255, 0.140f);
        m_Colors.SidebarActive = RGBA(255, 255, 255, 0.085f);
        m_Colors.Divider = RGBA(255, 255, 255, 0.055f);
        m_Colors.Danger = RGBA(232, 96, 88, 1.0f);
        m_Colors.GlassSurface = RGBA(255, 255, 255, 0.045f);
        m_Colors.GlassSheen = RGBA(255, 255, 255, 0.055f);
        m_Colors.GlassBorder = RGBA(255, 255, 255, 0.09f);
        m_Colors.Shadow = RGBA(0, 0, 0, 1.0f);

        m_Metrics.ContentWidth = 252.0f;
        m_Metrics.InputHeight = 38.0f;
        m_Metrics.InputRadius = 9.0f;
        m_Metrics.InputPaddingX = 13.0f;
        m_Metrics.ButtonHeight = 40.0f;
        m_Metrics.ButtonRadius = 9.0f;
        m_Metrics.IconButtonSize = 34.0f;
        m_Metrics.WindowControlSize = 26.0f;
        m_Metrics.TitleBarHeight = 42.0f;

        m_Animation.HoverSeconds = 0.10f;
        m_Animation.FocusSeconds = 0.12f;
        m_Animation.PressSeconds = 0.06f;
        m_Animation.TransitionSeconds = 0.17f;
        m_Animation.ScreenOutSeconds = 0.11f;
        m_Animation.ScreenInSeconds = 0.19f;

        m_Snow.ParticleCount = 95;
        m_Snow.MinSize = 1.1f;
        m_Snow.MaxSize = 2.7f;
        m_Snow.MinFallSpeed = 7.0f;
        m_Snow.MaxFallSpeed = 22.0f;
        m_Snow.MinOpacity = 0.05f;
        m_Snow.MaxOpacity = 0.26f;
        m_Snow.DriftAmount = 12.0f;
        m_Snow.DriftSpeed = 0.32f;
        m_Snow.BlurredFraction = 0.35f;
    }

    Theme& Theme::Get()
    {
        static Theme instance;
        return instance;
    }
}
