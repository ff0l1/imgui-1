#pragma once

#include "Loader/Product.hxx"
#include "UI/Animation/Animation.hxx"
#include "UI/Components/Button.hxx"
#include "imgui.h"

namespace UI::Loader
{
    class ProductCard
    {
    public:
        explicit ProductCard(::Loader::ProductTier tier);

        void Update(float deltaSeconds);
        bool Draw(ImVec2 origin, float width, const ::Loader::Product& product);

        static float Height();

    private:
        ::Loader::ProductTier m_Tier;
        Components::Button m_LoadButton;
        AnimatedFloat m_HoverT;
    };
}
