#include "WindowChrome.h"
#include "Layout.h"
#include "UI/Theme/Theme.h"
#include "UI/Icons/IconFont.h"
#include "Window/Win32Window.h"

#include "imgui.h"

namespace UI::Components
{
    namespace
    {
        constexpr float kEdgePadding = 14.0f;
        constexpr float kControlGap = 2.0f;
    }

    WindowChrome::WindowChrome(Window::Win32Window& window)
        : m_Window(window)
        , m_Dragging(false)
        , m_DragStartCursor{}
        , m_DragStartWindow{}
    {
    }

    void WindowChrome::Initialize()
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeMetrics& metrics = Theme::Get().Metrics();
        ImFont* font = Theme::Get().Typography().IconControl;

        IconButtonStyle style{};
        style.Size = metrics.WindowControlSize;
        style.Circular = true;
        style.FilledSurface = false;
        style.IdleColor = colors.TextSecondary;
        style.HoverColor = colors.TextPrimary;
        style.PressedColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.HoverSurface = colors.ControlHover;

        m_MinimizeButton = std::make_unique<IconButton>("chrome_minimize", ICON_MINIMIZE, font, style);

        IconButtonStyle closeStyle = style;
        closeStyle.IdleColor = colors.TextPrimary;
        closeStyle.HoverColor = ImVec4(0.91f, 0.30f, 0.28f, 1.0f);
        closeStyle.PressedColor = ImVec4(0.96f, 0.55f, 0.20f, 1.0f);
        closeStyle.HoverSurface = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        m_CloseButton = std::make_unique<IconButton>("chrome_close", ICON_CLOSE, font, closeStyle);
    }

    void WindowChrome::Update(float deltaSeconds)
    {
        m_MinimizeButton->Update(deltaSeconds);
        m_CloseButton->Update(deltaSeconds);
    }

    void WindowChrome::UpdateDragging(ImVec2 dragMin, ImVec2 dragMax)
    {
        const bool buttonHeld = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (!m_Dragging)
        {
            const bool overInteractive = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();
            if (!overInteractive && buttonHeld && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                && ImGui::IsMouseHoveringRect(dragMin, dragMax)
                && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            {
                m_Dragging = true;
                GetCursorPos(&m_DragStartCursor);
                m_DragStartWindow = m_Window.Position();
            }
            return;
        }

        if (!buttonHeld)
        {
            m_Dragging = false;
            return;
        }

        POINT cursor{};
        GetCursorPos(&cursor);
        m_Window.Move(
            m_DragStartWindow.x + (cursor.x - m_DragStartCursor.x),
            m_DragStartWindow.y + (cursor.y - m_DragStartCursor.y));
    }

    void WindowChrome::DrawTitle()
    {
        if (m_Title.empty())
            return;

        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();
        const float barHeight = Theme::Get().Metrics().TitleBarHeight;

        const float textHeight = TextHeight(typography.Small);
        DrawTextAt(
            ImVec2(kEdgePadding, (barHeight - textHeight) * 0.5f),
            m_Title.c_str(), typography.Small, colors.TextSecondary);
    }

    void WindowChrome::Draw(ImVec2 windowSize)
    {
        const ThemeMetrics& metrics = Theme::Get().Metrics();
        const float size = metrics.WindowControlSize;
        const float y = (metrics.TitleBarHeight - size) * 0.5f;

        DrawTitle();

        const ImVec2 closeOrigin(windowSize.x - kEdgePadding - size, y);
        const ImVec2 minimizeOrigin(closeOrigin.x - kControlGap - size, y);

        if (m_MinimizeButton->Draw(minimizeOrigin))
            m_Window.Minimize();

        if (m_CloseButton->Draw(closeOrigin))
            m_Window.RequestClose();

        UpdateDragging(ImVec2(0.0f, 0.0f), ImVec2(windowSize.x, metrics.TitleBarHeight));
    }

    void WindowChrome::DrawInHeader(ImVec2 headerMin, ImVec2 headerMax)
    {
        const float size = Theme::Get().Metrics().WindowControlSize;
        constexpr float kHeaderControlTop = 7.0f;
        constexpr float kHeaderControlInset = 5.0f;
        const float y = headerMin.y + kHeaderControlTop;
        const ImVec2 closeOrigin(headerMax.x - kHeaderControlInset - size, y);
        if (m_CloseButton->Draw(closeOrigin))
            m_Window.RequestClose();

    }
}
