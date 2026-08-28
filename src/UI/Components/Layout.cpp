#include "Layout.h"
#include "UI/Animation/RenderOpacity.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>

namespace UI::Components
{
    ImVec4 Mix(const ImVec4& a, const ImVec4& b, float t)
    {
        return ImVec4(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t);
    }

    float TextHeight(ImFont* font)
    {
        return font ? font->FontSize : ImGui::GetFontSize();
    }

    float TextWidth(const char* text, ImFont* font)
    {
        if (font)
            ImGui::PushFont(font);
        const float width = ImGui::CalcTextSize(text).x;
        if (font)
            ImGui::PopFont();
        return width;
    }

    void DrawTextAt(ImVec2 position, const char* text, ImFont* font, const ImVec4& color)
    {
        const ImVec2 snapped(std::floor(position.x), std::floor(position.y));

        if (font)
            ImGui::PushFont(font);
        ImGui::GetWindowDrawList()->AddText(snapped, ImGui::ColorConvertFloat4ToU32(ApplyOpacity(color)), text);
        if (font)
            ImGui::PopFont();
    }

    void DrawTextCentered(ImVec2 origin, float width, const char* text, ImFont* font, const ImVec4& color)
    {
        const float textWidth = TextWidth(text, font);
        DrawTextAt(ImVec2(origin.x + (width - textWidth) * 0.5f, origin.y), text, font, color);
    }

    void DrawGlyphCentered(ImVec2 center, const char* glyph, ImFont* font, const ImVec4& color)
    {
        if (!font)
            return;

        ImVec2 inkCenter(0.0f, 0.0f);

        unsigned int codepoint = 0;
        if (ImTextCharFromUtf8(&codepoint, glyph, nullptr) > 0)
        {
            if (const ImFontGlyph* found = font->FindGlyphNoFallback(static_cast<ImWchar>(codepoint)))
            {
                inkCenter.x = (found->X0 + found->X1) * 0.5f;
                inkCenter.y = (found->Y0 + found->Y1) * 0.5f;
            }
            else
            {
                return;
            }
        }

        const ImVec2 position(std::floor(center.x - inkCenter.x), std::floor(center.y - inkCenter.y));
        ImGui::GetWindowDrawList()->AddText(font, font->FontSize, position,
            ImGui::ColorConvertFloat4ToU32(ApplyOpacity(color)), glyph);
    }
}
