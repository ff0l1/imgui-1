#pragma once

#include <Windows.h>
#include <functional>
#include <string>
#include <vector>

namespace Window
{
    class Win32Window
    {
    public:
        Win32Window();
        ~Win32Window();

        Win32Window(const Win32Window&) = delete;
        Win32Window& operator=(const Win32Window&) = delete;

        bool Create(const std::wstring& title, int width, int height);
        void Show();
        bool PumpMessages();

        HWND Handle() const { return m_Hwnd; }
        int Width() const { return m_Width; }
        int Height() const { return m_Height; }
        bool IsMinimized() const { return m_Minimized; }

        void Minimize();
        void RequestClose();
        void Move(int x, int y);
        void ResizeCentered(int width, int height);
        void ResizeToMonitor();
        void SetIslandRegion(const std::vector<RECT>& islands, int radius);
        void ClearRegion();
        void SetDragRegion(RECT region) { m_DragRegion = region; m_HasDragRegion = true; }
        void ClearDragRegion() { m_HasDragRegion = false; }
        void SetInsertToggleEnabled(bool enabled);
        void SetMenuToggleVirtualKey(int virtualKey);
        bool ConsumeInsertToggleRequest();
        void SetVisible(bool visible);
        void SetOpacity(float opacity);
        POINT Position() const;

        void SetOnResize(std::function<void(int, int)> callback) { m_OnResize = std::move(callback); }

    private:
        static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void ApplyRoundedCornersAndShadow();

        HWND m_Hwnd;
        int m_Width;
        int m_Height;
        bool m_Minimized;
        bool m_HasDragRegion = false;
        RECT m_DragRegion{};
        bool m_RegionDragging = false;
        POINT m_RegionDragCursor{};
        POINT m_RegionDragWindow{};
        bool m_InsertToggleEnabled = false;
        bool m_InsertToggleRequested = false;
        int m_HotkeyVirtualKey = VK_INSERT;
        std::function<void(int, int)> m_OnResize;

        static constexpr wchar_t kWindowClassName[] = L"ImGuiMenuWindowClass";
    };
}
