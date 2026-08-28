#include "BackgroundOverlay.h"

#include <algorithm>
#include <cmath>

namespace Window
{
    namespace
    {
        constexpr wchar_t kOverlayClass[] = L"ImGuiMenuBackgroundOverlay";
        constexpr int kParticleCount = 125;
    }

    BackgroundOverlay::BackgroundOverlay()
        : m_Random(std::random_device{}())
    {
    }

    BackgroundOverlay::~BackgroundOverlay()
    {
        if (m_Hwnd)
            DestroyWindow(m_Hwnd);
    }

    bool BackgroundOverlay::Create(HWND owner)
    {
        const HMONITOR monitor = MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        GetMonitorInfoW(monitor, &info);
        m_Left = info.rcMonitor.left;
        m_Top = info.rcMonitor.top;
        m_Width = info.rcMonitor.right - info.rcMonitor.left;
        m_Height = info.rcMonitor.bottom - info.rcMonitor.top;

        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = WndProc;
        windowClass.hInstance = GetModuleHandleW(nullptr);
        windowClass.lpszClassName = kOverlayClass;
        RegisterClassExW(&windowClass);

        m_Hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
            kOverlayClass, L"", WS_POPUP, m_Left, m_Top, m_Width, m_Height,
            nullptr, nullptr, windowClass.hInstance, this);
        if (!m_Hwnd)
            return false;

        m_Particles.resize(kParticleCount);
        for (Particle& particle : m_Particles)
            ResetParticle(particle, true);
        Render();
        return true;
    }

    void BackgroundOverlay::Show()
    {
        ShowWindow(m_Hwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(m_Hwnd, HWND_TOPMOST, m_Left, m_Top, m_Width, m_Height,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    void BackgroundOverlay::Hide()
    {
        ShowWindow(m_Hwnd, SW_HIDE);
    }

    void BackgroundOverlay::ResetParticle(Particle& particle, bool anywhere)
    {
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        particle.X = unit(m_Random) * m_Width;
        particle.Y = anywhere ? unit(m_Random) * m_Height : -4.0f;
        particle.Speed = 28.0f + unit(m_Random) * 44.0f;
        particle.Size = 1.0f + unit(m_Random) * 2.2f;
        particle.Alpha = 0.20f + unit(m_Random) * 0.34f;
    }

    void BackgroundOverlay::Update(float deltaSeconds)
    {
        for (Particle& particle : m_Particles)
        {
            particle.Y += particle.Speed * deltaSeconds;
            if (particle.Y > m_Height + particle.Size)
                ResetParticle(particle, false);
        }
        Render();
    }

    void BackgroundOverlay::Render()
    {
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = m_Width;
        info.bmiHeader.biHeight = -m_Height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HDC screen = GetDC(nullptr);
        HDC memory = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateDIBSection(memory, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
        auto* pixels = static_cast<unsigned int*>(bits);

        const unsigned int tint = (89u << 24) | (6u << 16) | (4u << 8) | 2u;
        std::fill(pixels, pixels + static_cast<size_t>(m_Width) * m_Height, tint);

        for (const Particle& particle : m_Particles)
        {
            const int radius = std::max(1, static_cast<int>(std::round(particle.Size)));
            const int alpha = static_cast<int>(particle.Alpha * 255.0f);
            const int channel = alpha;
            const unsigned int color = (static_cast<unsigned int>(alpha) << 24)
                | (static_cast<unsigned int>(channel) << 16)
                | (static_cast<unsigned int>(channel) << 8)
                | static_cast<unsigned int>(channel);
            const int centerX = static_cast<int>(particle.X);
            const int centerY = static_cast<int>(particle.Y);
            for (int y = -radius; y <= radius; ++y)
            {
                for (int x = -radius; x <= radius; ++x)
                {
                    const int px = centerX + x;
                    const int py = centerY + y;
                    if (x * x + y * y <= radius * radius
                        && px >= 0 && px < m_Width && py >= 0 && py < m_Height)
                        pixels[static_cast<size_t>(py) * m_Width + px] = color;
                }
            }
        }

        POINT destination{ m_Left, m_Top };
        POINT source{};
        SIZE size{ m_Width, m_Height };
        const BYTE opacity = static_cast<BYTE>(std::clamp(m_Opacity, 0.0f, 1.0f) * 255.0f);
        BLENDFUNCTION blend{ AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA };
        UpdateLayeredWindow(m_Hwnd, screen, &destination, &size, memory, &source, 0, &blend, ULW_ALPHA);

        SelectObject(memory, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
    }

    LRESULT CALLBACK BackgroundOverlay::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_NCHITTEST)
            return HTTRANSPARENT;
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}
