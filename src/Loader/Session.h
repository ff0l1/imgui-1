#pragma once

#include "Product.h"

#include <string>
#include <vector>

namespace Loader
{
    class Session
    {
    public:
        void Begin(std::string username);
        void End();

        const std::string& Username() const { return m_Username; }
        const std::vector<Product>& Products() const { return m_Products; }

        std::vector<Product> ActiveProducts() const;
        int TotalDays() const { return TotalDaysRemaining(m_Products); }

        bool RedeemKey(const std::string& key);

    private:
        std::string m_Username;
        std::vector<Product> m_Products;
    };
}
