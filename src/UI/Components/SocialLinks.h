#pragma once

#include "IconButton.h"
#include <memory>

namespace UI::Components
{
    class SocialLinks
    {
    public:
        SocialLinks(std::string idPrefix);

        void Initialize();
        void Update(float deltaSeconds);
        void Draw(ImVec2 origin, float width);

        static float Height();

    private:
        std::string m_IdPrefix;
        std::unique_ptr<IconButton> m_DiscordButton;
        std::unique_ptr<IconButton> m_WebsiteButton;
    };
}
