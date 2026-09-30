#pragma once

#include <d3d11.h>
#include <cstdint>

namespace Renderer
{
    class Texture
    {
    public:
        Texture() = default;
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        bool Create(ID3D11Device* device, const uint8_t* rgbaPixels, int width, int height);
        void Release();

        ID3D11ShaderResourceView* View() const { return m_ShaderResourceView; }
        bool IsValid() const { return m_ShaderResourceView != nullptr; }

    private:
        ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
    };
}
