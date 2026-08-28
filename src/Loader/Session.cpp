#include "Session.h"

namespace Loader
{
    namespace
    {
        constexpr int kRedeemedDays = 30;
    }

    void Session::Begin(std::string username)
    {
        m_Username = std::move(username);
        m_Products = {
            Product{ ProductTier::Private, 360 },
            Product{ ProductTier::Public, 24 },
            Product{ ProductTier::Slotted, 0 },
        };
    }

    void Session::End()
    {
        m_Username.clear();
        m_Products.clear();
    }

    std::vector<Product> Session::ActiveProducts() const
    {
        std::vector<Product> active;
        for (const Product& product : m_Products)
        {
            if (product.IsActive())
                active.push_back(product);
        }
        return active;
    }

    bool Session::RedeemKey(const std::string& key)
    {
        if (key.empty())
            return false;

        for (Product& product : m_Products)
        {
            if (product.Tier == ProductTier::Slotted)
            {
                product.DaysRemaining += kRedeemedDays;
                return true;
            }
        }

        return false;
    }
}
