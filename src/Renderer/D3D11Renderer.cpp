#include "D3D11Renderer.h"

#include <d3d11.h>

namespace Renderer
{
    D3D11Renderer::D3D11Renderer()
        : m_Hwnd(nullptr)
        , m_Device(nullptr)
        , m_Context(nullptr)
        , m_SwapChain(nullptr)
        , m_RenderTargetView(nullptr)
        , m_SwapChainOccluded(false)
        , m_ResizeWidth(0)
        , m_ResizeHeight(0)
    {
    }

    D3D11Renderer::~D3D11Renderer()
    {
        Shutdown();
    }

    bool D3D11Renderer::Initialize(HWND hwnd)
    {
        m_Hwnd = hwnd;

        DXGI_SWAP_CHAIN_DESC swapChainDesc{};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hwnd;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc,
            &m_SwapChain, &m_Device, &featureLevel, &m_Context);

        if (hr == DXGI_ERROR_UNSUPPORTED)
        {
            hr = D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
                featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc,
                &m_SwapChain, &m_Device, &featureLevel, &m_Context);
        }

        if (hr != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void D3D11Renderer::Shutdown()
    {
        CleanupRenderTarget();

        if (m_SwapChain) { m_SwapChain->Release(); m_SwapChain = nullptr; }
        if (m_Context) { m_Context->Release(); m_Context = nullptr; }
        if (m_Device) { m_Device->Release(); m_Device = nullptr; }
    }

    void D3D11Renderer::CreateRenderTarget()
    {
        ID3D11Texture2D* backBuffer = nullptr;
        m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer)
        {
            m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
            backBuffer->Release();
        }
    }

    void D3D11Renderer::CleanupRenderTarget()
    {
        if (m_RenderTargetView)
        {
            m_RenderTargetView->Release();
            m_RenderTargetView = nullptr;
        }
    }

    void D3D11Renderer::ResizeSwapChain(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        m_ResizeWidth = static_cast<UINT>(width);
        m_ResizeHeight = static_cast<UINT>(height);

        CleanupRenderTarget();
        m_SwapChain->ResizeBuffers(0, m_ResizeWidth, m_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        m_ResizeWidth = 0;
        m_ResizeHeight = 0;
        CreateRenderTarget();
    }

    void D3D11Renderer::BeginFrame(const ImVec4& clearColor)
    {
        const float clearColorWithAlpha[4] = {
            clearColor.x * clearColor.w,
            clearColor.y * clearColor.w,
            clearColor.z * clearColor.w,
            clearColor.w
        };
        m_Context->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
        m_Context->ClearRenderTargetView(m_RenderTargetView, clearColorWithAlpha);
    }

    void D3D11Renderer::EndFrame(bool vSync)
    {
        HRESULT hr = m_SwapChain->Present(vSync ? 1 : 0, m_SwapChainOccluded ? DXGI_PRESENT_TEST : 0);
        m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }
}
