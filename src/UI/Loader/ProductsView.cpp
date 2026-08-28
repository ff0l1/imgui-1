#include "ProductsView.h"
#include "UI/Theme/Theme.h"
#include "UI/Components/Layout.h"

#include "imgui.h"

namespace UI::Loader
{
    namespace
    {
        constexpr float kHeaderHeight = 34.0f;
        constexpr float kHeaderToList = 20.0f;
        constexpr float kCardGap = 10.0f;
        constexpr float kAddTimeWidth = 80.0f;
        constexpr float kAddTimeHeight = 28.0f;
        constexpr float kEmptyStateTop = 26.0f;
    }

    ProductsView::ProductsView()
        : m_AddTimeButton("products_addtime", "Add time", Components::ButtonVariant::Outline, Components::ButtonShape::Pill)
    {
        m_Cards.emplace_back(::Loader::ProductTier::Private);
        m_Cards.emplace_back(::Loader::ProductTier::Public);
        m_Cards.emplace_back(::Loader::ProductTier::Slotted);
    }

    void ProductsView::Update(float deltaSeconds)
    {
        m_AddTimeButton.Update(deltaSeconds);
        for (ProductCard& card : m_Cards)
            card.Update(deltaSeconds);
    }

    ProductsAction ProductsView::Draw(ImVec2 origin, float width, const ::Loader::Session& session,
                                      const std::string& statusMessage)
    {
        ProductsAction action;
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        const float headingHeight = Components::TextHeight(typography.Heading);
        Components::DrawTextAt(ImVec2(origin.x, origin.y + (kHeaderHeight - headingHeight) * 0.5f),
            "Your products", typography.Heading, colors.TextPrimary);

        const ImVec2 addTimeOrigin(origin.x + width - kAddTimeWidth,
            origin.y + (kHeaderHeight - kAddTimeHeight) * 0.5f);

        if (m_AddTimeButton.Draw(addTimeOrigin, ImVec2(kAddTimeWidth, kAddTimeHeight)))
            action.AddTimeRequested = true;

        float y = origin.y + kHeaderHeight + kHeaderToList;
        int shown = 0;

        const std::vector<::Loader::Product>& products = session.Products();
        for (size_t index = 0; index < m_Cards.size() && index < products.size(); ++index)
        {
            if (!products[index].IsActive())
                continue;

            if (m_Cards[index].Draw(ImVec2(origin.x, y), width, products[index]))
            {
                action.LoadRequested = true;
                action.LoadedTier = products[index].Tier;
            }

            y += ProductCard::Height() + kCardGap;
            ++shown;
        }

        if (shown == 0)
        {
            Components::DrawTextCentered(ImVec2(origin.x, y + kEmptyStateTop), width,
                "No products yet. Redeem a key to add one.", typography.Small, colors.TextSecondary);
        }
        else if (!statusMessage.empty())
        {
            Components::DrawTextCentered(ImVec2(origin.x, y + 6.0f), width,
                statusMessage.c_str(), typography.Small, colors.TextSecondary);
        }

        return action;
    }
}
