#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "imgui.h"

namespace Renderer
{
    class D3D11Renderer
    {
    public:
        D3D11Renderer();
        ~D3D11Renderer();

        D3D11Renderer(const D3D11Renderer&) = delete;
        D3D11Renderer& operator=(const D3D11Renderer&) = delete;

        bool Initialize(HWND hwnd);
        void Shutdown();

        void ResizeSwapChain(int width, int height);
        void BeginFrame(const ImVec4& clearColor);
        void EndFrame(bool vSync = true);

        ID3D11Device* Device() const { return m_Device; }
        ID3D11DeviceContext* Context() const { return m_Context; }

    private:
        void CreateRenderTarget();
        void CleanupRenderTarget();

        HWND m_Hwnd;
        ID3D11Device* m_Device;
        ID3D11DeviceContext* m_Context;
        IDXGISwapChain* m_SwapChain;
        ID3D11RenderTargetView* m_RenderTargetView;
        bool m_SwapChainOccluded;
        UINT m_ResizeWidth;
        UINT m_ResizeHeight;
    };
}
