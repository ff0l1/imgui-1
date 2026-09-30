#include "AuthLayout.hxx"
#include "UI/Components/Layout.hxx"
#include "UI/Theme/Theme.hxx"

namespace UI::Authentication::Layout
{
    float LogoHeight()
    {
        return Components::TextHeight(Theme::Get().Typography().Heading);
    }

    void DrawBrand(ImVec2 origin, float width)
    {
        Components::DrawTextCentered(
            origin, width, BrandName,
            Theme::Get().Typography().Heading,
            Theme::Get().Colors().TextPrimary);
    }
}
