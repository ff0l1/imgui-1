#include "AddTimeView.h"
#include "UI/Theme/Theme.h"
#include "UI/Components/Layout.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include <cstring>

namespace UI::Loader
{
    namespace
    {
        constexpr float kHeadingToBody = 10.0f;
        constexpr float kBodyToLabel = 22.0f;
        constexpr float kLabelToInput = 8.0f;
        constexpr float kInputToButton = 18.0f;
        constexpr float kButtonToStatus = 12.0f;
        constexpr float kStatusToBack = 12.0f;
        constexpr float kRedeemHeight = 38.0f;

        const char* kDescription =
            "Redeem any Private, Public, or Slotted key. Time is added; "
            "a different product switches your account to that SKU.";
    }

    AddTimeView::AddTimeView()
        : m_Key{}
        , m_KeyInput("addtime_key", "Enter your key")
        , m_RedeemButton("addtime_redeem", "Redeem", Components::ButtonVariant::Filled, Components::ButtonShape::Pill)
        , m_BackLink("Back")
    {
    }

    void AddTimeView::Reset()
    {
        std::memset(m_Key, 0, sizeof(m_Key));
        m_Status.clear();
    }

    void AddTimeView::SetStatus(std::string message)
    {
        m_Status = std::move(message);
    }

    void AddTimeView::Update(float deltaSeconds)
    {
        m_KeyInput.Update(deltaSeconds);
        m_RedeemButton.Update(deltaSeconds);
        m_BackLink.Update(deltaSeconds);

        m_RedeemButton.SetEnabled(m_Key[0] != '\0');
    }

    AddTimeAction AddTimeView::Draw(ImVec2 origin, float width)
    {
        AddTimeAction action;
        const ThemeColors& colors = Theme::Get().Colors();
        const ThemeTypography& typography = Theme::Get().Typography();

        float y = origin.y;

        Components::DrawTextAt(ImVec2(origin.x, y), "Add time", typography.Heading, colors.TextPrimary);
        y += Components::TextHeight(typography.Heading) + kHeadingToBody;

        ImGui::PushFont(typography.Small);
        ImGui::SetCursorScreenPos(ImVec2(origin.x, y));
        ImGui::PushStyleColor(ImGuiCol_Text, ApplyOpacity(colors.TextSecondary));
        ImGui::PushTextWrapPos(origin.x + width);
        ImGui::TextUnformatted(kDescription);
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
        const float descriptionHeight = ImGui::GetItemRectSize().y;
        ImGui::PopFont();

        y += descriptionHeight + kBodyToLabel;

        Components::DrawTextAt(ImVec2(origin.x, y), "Redeem key", typography.Small, colors.TextSecondary);
        y += Components::TextHeight(typography.Small) + kLabelToInput;

        m_KeyInput.Draw(ImVec2(origin.x, y), width, m_Key, sizeof(m_Key));
        y += Components::TextInput::Height() + kInputToButton;

        if (m_RedeemButton.Draw(ImVec2(origin.x, y), ImVec2(width, kRedeemHeight)))
        {
            action.RedeemRequested = true;
            action.Key = m_Key;
        }
        y += kRedeemHeight + kButtonToStatus;

        if (!m_Status.empty())
        {
            Components::DrawTextCentered(ImVec2(origin.x, y), width, m_Status.c_str(), typography.Small, colors.Danger);
        }
        y += Components::TextHeight(typography.Small) + kStatusToBack;

        action.BackRequested = m_BackLink.Draw(ImVec2(origin.x, y), width);

        return action;
    }
}
