#pragma once

#include <Windows.h>
#include <random>
#include <vector>
#include <chrono>

namespace Window
{
    class BackgroundOverlay
    {
    public:
        BackgroundOverlay();
        ~BackgroundOverlay();

        bool Create(HWND owner);
        void Show();
        void Hide();
        void SetOpacity(float opacity) { m_Opacity = opacity; }
        void Update(float deltaSeconds);
        HWND Handle() const { return m_Hwnd; }

    private:
        struct Particle
        {
            float X;
            float Y;
            float Speed;
            float Size;
            float Alpha;
        };

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        void ResetParticle(Particle& particle, bool anywhere);
        void Render();

        HWND m_Hwnd = nullptr;
        int m_Left = 0;
        int m_Top = 0;
        int m_Width = 0;
        int m_Height = 0;
        std::vector<Particle> m_Particles;
        std::mt19937 m_Random;
        float m_Opacity = 1.0f;
        std::chrono::steady_clock::time_point m_LastUpdate{};
    };
}
