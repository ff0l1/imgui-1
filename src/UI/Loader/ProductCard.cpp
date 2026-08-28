#include "ProductCard.h"
#include "UI/Theme/Theme.h"
#include "UI/Components/Layout.h"
#include "UI/Animation/RenderOpacity.h"
#include "UI/Icons/IconFont.h"

#include "imgui.h"
#include <string>

namespace UI::Loader
{
    namespace
    {
        constexpr float kCardHeight = 68.0f;
        constexpr float kCardRadius = 12.0f;
        constexpr float kPaddingX = 16.0f;
        constexpr float kLoadButtonWidth = 78.0f;
        constexpr float kLoadButtonHeight = 32.0f;
    }

    ProductCard::ProductCard(::Loader::ProductTier tier)
        : m_Tier(tier)
        , m_LoadButton(std::string("load_") + ::Loader::TierName(tier), "Load", Components::ButtonVariant::Filled, Components::ButtonShape::Pill)
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
    {
    }

    float ProductCard::Height()
    {
        return kCardHeight;
    }

    void ProductCard::Update(float deltaSeconds)
    {
        m_LoadButton.Update(deltaSeconds);
        m_HoverT.Update(deltaSeconds);
    }

    bool ProductCard::Draw(ImVec2 origin, float width, const ::Loader::Product& product)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        const ImVec2 max(origin.x + width, origin.y + kCardHeight);
        const bool hovered = ImGui::IsMouseHoveringRect(origin, max);
        m_HoverT.SetTarget(hovered ? 1.0f : 0.0f);

        const ImVec4 surface = Components::Mix(colors.CardSurface, colors.CardSurfaceHover, m_HoverT.Value());

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(origin, max,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(surface)), kCardRadius);
        drawList->AddRect(origin, max,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.CardBorder)), kCardRadius, 0, 1.0f);

        const float nameHeight = Components::TextHeight(typography.BodyMedium);
        const float metaHeight = Components::TextHeight(typography.Small);
        const float blockHeight = nameHeight + 6.0f + metaHeight;
        const float blockTop = origin.y + (kCardHeight - blockHeight) * 0.5f;

        Components::DrawTextAt(ImVec2(origin.x + kPaddingX, blockTop),
            ::Loader::TierName(m_Tier), typography.BodyMedium, colors.TextPrimary);

        const ImVec2 metaOrigin(origin.x + kPaddingX, blockTop + nameHeight + 6.0f);
        const float glyphWidth = Components::TextWidth(ICON_SCHEDULE, typography.IconControl);

        Components::DrawGlyphCentered(
            ImVec2(metaOrigin.x + glyphWidth * 0.5f, metaOrigin.y + Components::TextHeight(typography.Small) * 0.5f),
            ICON_SCHEDULE, typography.IconControl, colors.TextSecondary);

        Components::DrawTextAt(ImVec2(metaOrigin.x + glyphWidth + 6.0f, metaOrigin.y),
            ::Loader::FormatRemaining(product).c_str(), typography.Small, colors.TextSecondary);

        const ImVec2 buttonOrigin(
            max.x - kPaddingX - kLoadButtonWidth,
            origin.y + (kCardHeight - kLoadButtonHeight) * 0.5f);

        return m_LoadButton.Draw(buttonOrigin, ImVec2(kLoadButtonWidth, kLoadButtonHeight));
    }
}
