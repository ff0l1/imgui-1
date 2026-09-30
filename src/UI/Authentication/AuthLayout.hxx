#pragma once

#include "imgui.h"

namespace UI::Authentication
{
    struct ViewAction
    {
        bool SwitchRequested = false;
        bool SubmitRequested = false;
    };
}

namespace UI::Authentication::Layout
{
    constexpr const char* BrandName = "imgui-menu";
    constexpr float LogoToFields = 24.0f;
    constexpr float FieldGap = 9.0f;
    constexpr float FieldsToButton = 18.0f;
    constexpr float ButtonToLink = 16.0f;
    constexpr float LinkToSocial = 22.0f;

    float LogoHeight();
    void DrawBrand(ImVec2 origin, float width);
}
