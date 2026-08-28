#pragma once

#include "ProductCard.h"
#include "Loader/Session.h"
#include "UI/Components/Button.h"
#include "imgui.h"

#include <string>
#include <vector>

namespace UI::Loader
{
    struct ProductsAction
    {
        bool AddTimeRequested = false;
        bool LoadRequested = false;
        ::Loader::ProductTier LoadedTier = ::Loader::ProductTier::Public;
    };

    class ProductsView
    {
    public:
        ProductsView();

        void Update(float deltaSeconds);
        ProductsAction Draw(ImVec2 origin, float width, const ::Loader::Session& session,
                            const std::string& statusMessage);

    private:
        Components::Button m_AddTimeButton;
        std::vector<ProductCard> m_Cards;
    };
}
