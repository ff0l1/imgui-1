#include "Button.h"
#include "Layout.h"
#include "UI/Theme/Theme.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"

namespace UI::Components
{
    namespace
    {
        struct ButtonPalette
        {
            ImVec4 Surface;
            ImVec4 Border;
            ImVec4 Label;
            bool HasBorder = false;
        };

        ButtonPalette ResolvePalette(ButtonVariant variant, bool enabled, float hover, float press)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            ButtonPalette palette{};

            switch (variant)
            {
            case ButtonVariant::Filled:
                palette.Surface = Mix(colors.Accent, colors.AccentHover, hover);
                palette.Surface = Mix(palette.Surface, colors.AccentPressed, press);
                palette.Label = colors.AccentText;
                break;

            case ButtonVariant::Outline:
                palette.Surface = Mix(colors.OutlineSurface, colors.OutlineSurfaceHover, hover);
                palette.Border = colors.OutlineBorder;
                palette.Label = Mix(colors.TextSecondary, colors.TextPrimary, hover);
                palette.HasBorder = true;
                break;

            case ButtonVariant::Danger:
                palette.Surface = Mix(colors.OutlineSurface, colors.Danger, hover * 0.22f);
                palette.Border = Mix(colors.OutlineBorder, colors.Danger, hover * 0.55f);
                palette.Label = Mix(colors.TextSecondary, colors.Danger, hover);
                palette.HasBorder = true;
                break;
            }

            if (!enabled)
            {
                palette.Surface = colors.AccentDisabled;
                palette.Border = colors.Border;
                palette.Label = colors.TextDisabled;
            }

            return palette;
        }
    }

    Button::Button(std::string id, std::string label, ButtonVariant variant, ButtonShape shape)
        : m_Id(std::move(id))
        , m_Label(std::move(label))
        , m_Variant(variant)
        , m_Shape(shape)
        , m_Enabled(true)
        , m_HoverT(0.0f, Theme::Get().Animation().HoverSeconds)
        , m_PressT(0.0f, Theme::Get().Animation().PressSeconds)
    {
    }

    float Button::Height()
    {
        return Theme::Get().Metrics().ButtonHeight;
    }

    void Button::Update(float deltaSeconds)
    {
        m_HoverT.Update(deltaSeconds);
        m_PressT.Update(deltaSeconds);
    }

    bool Button::Draw(ImVec2 origin, float width)
    {
        return Draw(origin, ImVec2(width, Theme::Get().Metrics().ButtonHeight));
    }

    bool Button::Draw(ImVec2 origin, ImVec2 size)
    {
        const ThemeMetrics& metrics = Theme::Get().Metrics();
        const ThemeTypography& typography = Theme::Get().Typography();

        ImGui::PushID(m_Id.c_str());
        ImGui::SetCursorScreenPos(origin);
        const bool clicked = ImGui::InvisibleButton("##button", size);
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();
        ImGui::PopID();

        m_HoverT.SetTarget(m_Enabled && hovered ? 1.0f : 0.0f);
        m_PressT.SetTarget(m_Enabled && held ? 1.0f : 0.0f);

        if (m_Enabled && hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        const ButtonPalette palette = ResolvePalette(m_Variant, m_Enabled, m_HoverT.Value(), m_PressT.Value());
        const float radius = (m_Shape == ButtonShape::Pill) ? size.y * 0.5f : metrics.ButtonRadius;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 max(origin.x + size.x, origin.y + size.y);

        drawList->AddRectFilled(origin, max,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(palette.Surface)), radius);

        if (palette.HasBorder)
        {
            drawList->AddRect(origin, max,
                ImGui::ColorConvertFloat4ToU32(ApplyOpacity(palette.Border)), radius, 0, 1.0f);
        }

        const float textHeight = TextHeight(typography.BodyMedium);
        const ImVec2 textOrigin(origin.x, origin.y + (size.y - textHeight) * 0.5f);
        DrawTextCentered(textOrigin, size.x, m_Label.c_str(), typography.BodyMedium, palette.Label);

        return clicked && m_Enabled;
    }
}
