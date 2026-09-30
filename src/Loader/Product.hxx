#pragma once

#include <string>
#include <vector>

namespace Loader
{
    enum class ProductTier
    {
        Private,
        Public,
        Slotted
    };

    struct Product
    {
        ProductTier Tier = ProductTier::Public;
        int DaysRemaining = 0;

        bool IsActive() const { return DaysRemaining > 0; }
    };

    const char* TierName(ProductTier tier);
    const char* TierDescription(ProductTier tier);
    std::string FormatRemaining(const Product& product);

    int TotalDaysRemaining(const std::vector<Product>& products);
}
