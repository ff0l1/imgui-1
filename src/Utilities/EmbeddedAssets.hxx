#pragma once

#include <cstddef>
#include <cstdint>

namespace Utilities
{
    struct EmbeddedAsset
    {
        const uint8_t* Data = nullptr;
        size_t Size = 0;

        bool IsValid() const { return Data != nullptr && Size > 0; }
    };

    EmbeddedAsset LoadEmbeddedAsset(int resourceId);
}
