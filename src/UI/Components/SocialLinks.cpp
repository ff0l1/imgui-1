#include "SocialLinks.h"
#include "UI/Theme/Theme.h"
#include "UI/Icons/IconFont.h"
#include "Utilities/ShellUtils.h"

#include "imgui.h"

namespace UI::Components
{
    namespace
    {
        constexpr float kGap = 10.0f;
        constexpr const char* kDiscordURL = "https://example.com";
        constexpr const char* kWebsiteURL = "https://example.com";
    }

    SocialLinks::SocialLinks(std::string idPrefix)
        : m_IdPrefix(std::move(idPrefix))
    {
    }

    float SocialLinks::Height()
    {
        return Theme::Get().Metrics().IconButtonSize;
    }

    void SocialLinks::Initialize()
    {
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        IconButtonStyle style{};
        style.Size = Theme::Get().Metrics().IconButtonSize;
        style.Circular = true;
        style.FilledSurface = false;
        style.IdleColor = colors.TextSecondary;
        style.HoverColor = colors.TextPrimary;
        style.HoverSurface = colors.ControlHover;

        m_DiscordButton = std::make_unique<IconButton>(m_IdPrefix + "_discord", ICON_DISCORD, typography.IconBrand, style);
        m_WebsiteButton = std::make_unique<IconButton>(m_IdPrefix + "_website", ICON_GLOBE, typography.IconAction, style);
    }

    void SocialLinks::Update(float deltaSeconds)
    {
        m_DiscordButton->Update(deltaSeconds);
        m_WebsiteButton->Update(deltaSeconds);
    }

    void SocialLinks::Draw(ImVec2 origin, float width)
    {
        const float size = Theme::Get().Metrics().IconButtonSize;
        const float totalWidth = size * 2.0f + kGap;
        const float startX = origin.x + (width - totalWidth) * 0.5f;

        if (m_DiscordButton->Draw(ImVec2(startX, origin.y)))
            Utilities::OpenURL(kDiscordURL);

        if (m_WebsiteButton->Draw(ImVec2(startX + size + kGap, origin.y)))
            Utilities::OpenURL(kWebsiteURL);
    }
}
