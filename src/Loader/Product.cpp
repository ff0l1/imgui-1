#include "Product.h"

namespace Loader
{
    const char* TierName(ProductTier tier)
    {
        switch (tier)
        {
        case ProductTier::Private: return "Private";
        case ProductTier::Public:  return "Public";
        case ProductTier::Slotted: return "Slotted";
        }
        return "Unknown";
    }

    const char* TierDescription(ProductTier tier)
    {
        switch (tier)
        {
        case ProductTier::Private: return "Full feature set, private build";
        case ProductTier::Public:  return "Standard build, public release";
        case ProductTier::Slotted: return "Limited slots, rotating access";
        }
        return "";
    }

    std::string FormatRemaining(const Product& product)
    {
        if (!product.IsActive())
            return "No time remaining";

        if (product.DaysRemaining == 1)
            return "1 day left";

        return std::to_string(product.DaysRemaining) + " days left";
    }

    int TotalDaysRemaining(const std::vector<Product>& products)
    {
        int total = 0;
        for (const Product& product : products)
            total += product.DaysRemaining;
        return total;
    }
}
