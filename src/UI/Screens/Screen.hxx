#pragma once

#include "imgui.h"

struct ID3D11Device;

namespace UI::Screens
{
    class Screen
    {
    public:
        virtual ~Screen() = default;

        virtual void Initialize(ID3D11Device* device) = 0;
        virtual void Update(float deltaSeconds) = 0;
        virtual void Draw(ImVec2 regionMin, ImVec2 regionSize) = 0;
    };
}
