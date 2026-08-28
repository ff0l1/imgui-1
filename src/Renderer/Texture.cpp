#include "Texture.h"

#include <utility>

namespace Renderer
{
    Texture::~Texture()
    {
        Release();
    }

    Texture::Texture(Texture&& other) noexcept
        : m_ShaderResourceView(other.m_ShaderResourceView)
    {
        other.m_ShaderResourceView = nullptr;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_ShaderResourceView = other.m_ShaderResourceView;
            other.m_ShaderResourceView = nullptr;
        }
        return *this;
    }

    void Texture::Release()
    {
        if (m_ShaderResourceView)
        {
            m_ShaderResourceView->Release();
            m_ShaderResourceView = nullptr;
        }
    }

    bool Texture::Create(ID3D11Device* device, const uint8_t* rgbaPixels, int width, int height)
    {
        Release();

        if (!device || !rgbaPixels || width <= 0 || height <= 0)
            return false;

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = static_cast<UINT>(width);
        desc.Height = static_cast<UINT>(height);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initialData{};
        initialData.pSysMem = rgbaPixels;
        initialData.SysMemPitch = static_cast<UINT>(width * 4);

        ID3D11Texture2D* texture = nullptr;
        if (FAILED(device->CreateTexture2D(&desc, &initialData, &texture)) || !texture)
            return false;

        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
        viewDesc.Format = desc.Format;
        viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = 1;

        const HRESULT result = device->CreateShaderResourceView(texture, &viewDesc, &m_ShaderResourceView);
        texture->Release();

        return SUCCEEDED(result);
    }
}
