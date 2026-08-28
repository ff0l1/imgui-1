#include "Win32Window.h"

#include "imgui_impl_win32.h"
#include <dwmapi.h>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
{
    namespace
    {
        constexpr int kInsertHotkeyId = 1;
    }

    constexpr wchar_t Win32Window::kWindowClassName[];

    Win32Window::Win32Window()
        : m_Hwnd(nullptr)
        , m_Width(0)
        , m_Height(0)
        , m_Minimized(false)
    {
    }

    Win32Window::~Win32Window()
    {
        if (m_Hwnd)
        {
            DestroyWindow(m_Hwnd);
            UnregisterClassW(kWindowClassName, GetModuleHandleW(nullptr));
        }
    }

    bool Win32Window::Create(const std::wstring& title, int width, int height)
    {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = &Win32Window::WndProcStatic;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kWindowClassName;
        RegisterClassExW(&wc);

        m_Width = width;
        m_Height = height;

        const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        const int x = (screenWidth - width) / 2;
        const int y = (screenHeight - height) / 2;

        m_Hwnd = CreateWindowExW(
            WS_EX_APPWINDOW | WS_EX_LAYERED,
            kWindowClassName,
            title.c_str(),
            WS_POPUP,
            x, y, width, height,
            nullptr, nullptr, wc.hInstance, this);

        if (!m_Hwnd)
            return false;

        SetLayeredWindowAttributes(m_Hwnd, 0, 255, LWA_ALPHA);
        ApplyRoundedCornersAndShadow();
        return true;
    }

    void Win32Window::ApplyRoundedCornersAndShadow()
    {
        DWORD cornerPreference = DWMWCP_ROUND;
        DwmSetWindowAttribute(m_Hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

        MARGINS margins{ 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(m_Hwnd, &margins);
    }

    void Win32Window::Show()
    {
        ShowWindow(m_Hwnd, SW_SHOWDEFAULT);
        UpdateWindow(m_Hwnd);
    }

    bool Win32Window::PumpMessages()
    {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return false;

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return true;
    }

    void Win32Window::Minimize()
    {
        ShowWindow(m_Hwnd, SW_MINIMIZE);
    }

    void Win32Window::RequestClose()
    {
        PostMessageW(m_Hwnd, WM_CLOSE, 0, 0);
    }

    void Win32Window::Move(int x, int y)
    {
        SetWindowPos(m_Hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::ResizeCentered(int width, int height)
    {
        RECT current{};
        GetWindowRect(m_Hwnd, &current);

        const int centerX = current.left + (current.right - current.left) / 2;
        const int centerY = current.top + (current.bottom - current.top) / 2;

        SetWindowPos(m_Hwnd, nullptr,
            centerX - width / 2, centerY - height / 2,
            width, height,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::ResizeToMonitor()
    {
        const HMONITOR monitor = MonitorFromWindow(m_Hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        GetMonitorInfoW(monitor, &info);
        SetWindowPos(m_Hwnd, HWND_TOP,
            info.rcMonitor.left, info.rcMonitor.top,
            info.rcMonitor.right - info.rcMonitor.left,
            info.rcMonitor.bottom - info.rcMonitor.top,
            SWP_NOACTIVATE);
    }

    void Win32Window::SetInsertToggleEnabled(bool enabled)
    {
        if (enabled == m_InsertToggleEnabled)
            return;

        m_InsertToggleEnabled = enabled;
        if (enabled)
        {
            RegisterHotKey(m_Hwnd, kInsertHotkeyId, MOD_NOREPEAT, m_HotkeyVirtualKey);
        }
        else
        {
            UnregisterHotKey(m_Hwnd, kInsertHotkeyId);
            m_InsertToggleRequested = false;
            SetOpacity(1.0f);
            SetVisible(true);
        }
    }

    void Win32Window::SetMenuToggleVirtualKey(int virtualKey)
    {
        if (virtualKey == m_HotkeyVirtualKey)
            return;
        if (m_InsertToggleEnabled)
            UnregisterHotKey(m_Hwnd, kInsertHotkeyId);
        m_HotkeyVirtualKey = virtualKey;
        if (m_InsertToggleEnabled && virtualKey != 0)
            RegisterHotKey(m_Hwnd, kInsertHotkeyId, MOD_NOREPEAT, virtualKey);
    }

    bool Win32Window::ConsumeInsertToggleRequest()
    {
        const bool requested = m_InsertToggleRequested;
        m_InsertToggleRequested = false;
        return requested;
    }

    void Win32Window::SetVisible(bool visible)
    {
        ShowWindow(m_Hwnd, visible ? SW_SHOW : SW_HIDE);
        if (visible)
            SetForegroundWindow(m_Hwnd);
    }

    void Win32Window::SetOpacity(float opacity)
    {
        const float clamped = opacity < 0.0f ? 0.0f : (opacity > 1.0f ? 1.0f : opacity);
        SetLayeredWindowAttributes(m_Hwnd, 0, static_cast<BYTE>(clamped * 255.0f), LWA_ALPHA);
    }

    void Win32Window::SetIslandRegion(const std::vector<RECT>& islands, int radius)
    {
        constexpr COLORREF kNoBorder = 0xFFFFFFFE;
        DwmSetWindowAttribute(m_Hwnd, DWMWA_BORDER_COLOR, &kNoBorder, sizeof(kNoBorder));

        HRGN combined = CreateRectRgn(0, 0, 0, 0);
        for (const RECT& island : islands)
        {
            HRGN part = CreateRoundRectRgn(
                island.left, island.top, island.right + 1, island.bottom + 1,
                radius * 2, radius * 2);
            CombineRgn(combined, combined, part, RGN_OR);
            DeleteObject(part);
        }

        if (SetWindowRgn(m_Hwnd, combined, TRUE) == 0)
            DeleteObject(combined);
    }

    void Win32Window::ClearRegion()
    {
        SetWindowRgn(m_Hwnd, nullptr, TRUE);
        const COLORREF defaultBorder = 0xFFFFFFFF;
        DwmSetWindowAttribute(m_Hwnd, DWMWA_BORDER_COLOR, &defaultBorder, sizeof(defaultBorder));
    }

    POINT Win32Window::Position() const
    {
        RECT rect{};
        GetWindowRect(m_Hwnd, &rect);
        return POINT{ rect.left, rect.top };
    }

    LRESULT CALLBACK Win32Window::WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            const CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
        }

        Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (window)
            return window->WndProc(hwnd, msg, wParam, lParam);

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    LRESULT Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (m_HasDragRegion && msg == WM_LBUTTONDOWN)
        {
            POINT point{ static_cast<LONG>(LOWORD(lParam)), static_cast<LONG>(HIWORD(lParam)) };
            if (PtInRect(&m_DragRegion, point))
            {
                m_RegionDragging = true;
                GetCursorPos(&m_RegionDragCursor);
                m_RegionDragWindow = Position();
                SetCapture(hwnd);
                return 0;
            }
        }
        if (m_RegionDragging && msg == WM_MOUSEMOVE)
        {
            POINT cursor{};
            GetCursorPos(&cursor);
            Move(m_RegionDragWindow.x + cursor.x - m_RegionDragCursor.x,
                m_RegionDragWindow.y + cursor.y - m_RegionDragCursor.y);
            return 0;
        }
        if (m_RegionDragging && msg == WM_LBUTTONUP)
        {
            m_RegionDragging = false;
            ReleaseCapture();
            return 0;
        }

        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SYSCHAR:
        case WM_MENUCHAR:
            if (m_HasDragRegion)
                return 0;
            break;
        case WM_HOTKEY:
            if (m_InsertToggleEnabled && wParam == kInsertHotkeyId)
            {
                m_InsertToggleRequested = true;
                return 0;
            }
            break;
        case WM_SIZE:
        {
            m_Minimized = (wParam == SIZE_MINIMIZED);
            if (!m_Minimized)
            {
                m_Width = LOWORD(lParam);
                m_Height = HIWORD(lParam);
                if (m_OnResize)
                    m_OnResize(m_Width, m_Height);
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
