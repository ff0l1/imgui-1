#include "AccountView.h"
#include "UI/Theme/Theme.h"
#include "UI/Components/Layout.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include <string>

namespace UI::Loader
{
    namespace
    {
        constexpr float kHeadingToPanel = 18.0f;
        constexpr float kPanelRadius = 12.0f;
        constexpr float kPanelPadding = 16.0f;
        constexpr float kRowHeight = 30.0f;
        constexpr float kSectionGap = 18.0f;
        constexpr float kLogOutWidth = 104.0f;
        constexpr float kLogOutHeight = 34.0f;

        void DrawRow(ImVec2 origin, float width, const char* label, const std::string& value, const ImVec4& valueColor)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();

            const float labelHeight = Components::TextHeight(typography.Small);
            const float top = origin.y + (kRowHeight - labelHeight) * 0.5f;

            Components::DrawTextAt(ImVec2(origin.x, top), label, typography.Small, colors.TextSecondary);

            const float valueWidth = Components::TextWidth(value.c_str(), typography.BodyMedium);
            const float valueHeight = Components::TextHeight(typography.BodyMedium);
            Components::DrawTextAt(
                ImVec2(origin.x + width - valueWidth, origin.y + (kRowHeight - valueHeight) * 0.5f),
                value.c_str(), typography.BodyMedium, valueColor);
        }
    }

    AccountView::AccountView()
        : m_LogOutButton("account_logout", "Log out", Components::ButtonVariant::Danger, Components::ButtonShape::Pill)
        , m_SocialLinks("account")
    {
    }

    void AccountView::Initialize()
    {
        m_SocialLinks.Initialize();
    }

    void AccountView::Update(float deltaSeconds)
    {
        m_LogOutButton.Update(deltaSeconds);
        m_SocialLinks.Update(deltaSeconds);
    }

    bool AccountView::Draw(ImVec2 origin, float width, const ::Loader::Session& session)
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        float y = origin.y;

        Components::DrawTextAt(ImVec2(origin.x, y), "Account", typography.Heading, colors.TextPrimary);
        y += Components::TextHeight(typography.Heading) + kHeadingToPanel;

        const std::vector<::Loader::Product> products = session.ActiveProducts();
        const float panelHeight = kPanelPadding * 2.0f + kRowHeight * static_cast<float>(2 + products.size());

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 panelMin(origin.x, y);
        const ImVec2 panelMax(origin.x + width, y + panelHeight);
        drawList->AddRectFilled(panelMin, panelMax,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.CardSurface)), kPanelRadius);
        drawList->AddRect(panelMin, panelMax,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.CardBorder)), kPanelRadius, 0, 1.0f);

        const float rowWidth = width - kPanelPadding * 2.0f;
        float rowY = y + kPanelPadding;

        DrawRow(ImVec2(origin.x + kPanelPadding, rowY), rowWidth, "Username",
            session.Username().empty() ? std::string("-") : session.Username(), colors.TextPrimary);
        rowY += kRowHeight;

        const ImVec2 dividerStart(origin.x + kPanelPadding, rowY);
        drawList->AddLine(dividerStart, ImVec2(dividerStart.x + rowWidth, dividerStart.y),
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.Divider)), 1.0f);

        for (const ::Loader::Product& product : products)
        {
            const ImVec4 valueColor = product.IsActive() ? colors.TextPrimary : colors.TextDisabled;
            DrawRow(ImVec2(origin.x + kPanelPadding, rowY), rowWidth,
                ::Loader::TierName(product.Tier), ::Loader::FormatRemaining(product), valueColor);
            rowY += kRowHeight;
        }

        drawList->AddLine(ImVec2(origin.x + kPanelPadding, rowY),
            ImVec2(origin.x + kPanelPadding + rowWidth, rowY),
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(colors.Divider)), 1.0f);

        DrawRow(ImVec2(origin.x + kPanelPadding, rowY), rowWidth, "Total time remaining",
            std::to_string(session.TotalDays()) + " days", colors.TextPrimary);

        y += panelHeight + kSectionGap;

        const float socialSize = Components::SocialLinks::Height();
        const float socialWidth = socialSize * 2.0f + 10.0f;
        m_SocialLinks.Draw(ImVec2(origin.x + width - socialWidth, y + (kLogOutHeight - socialSize) * 0.5f), socialWidth);

        return m_LogOutButton.Draw(ImVec2(origin.x, y), ImVec2(kLogOutWidth, kLogOutHeight));
    }
}
