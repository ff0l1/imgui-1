#include "EmbeddedAssets.h"

#include <Windows.h>

namespace Utilities
{
    EmbeddedAsset LoadEmbeddedAsset(int resourceId)
    {
        const HMODULE module = GetModuleHandleW(nullptr);
        const HRSRC info = FindResourceW(module, MAKEINTRESOURCEW(resourceId), RT_RCDATA);
        if (!info)
            return {};

        const HGLOBAL handle = LoadResource(module, info);
        if (!handle)
            return {};

        EmbeddedAsset asset;
        asset.Data = static_cast<const uint8_t*>(LockResource(handle));
        asset.Size = SizeofResource(module, info);
        return asset;
    }
}
