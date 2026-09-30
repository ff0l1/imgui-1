#pragma once

#include "imgui.h"

struct ImFont;

namespace UI::Components
{
    ImVec4 Mix(const ImVec4& a, const ImVec4& b, float t);

    float TextHeight(ImFont* font);
    float TextWidth(const char* text, ImFont* font);

    void DrawTextAt(ImVec2 position, const char* text, ImFont* font, const ImVec4& color);
    void DrawTextCentered(ImVec2 origin, float width, const char* text, ImFont* font, const ImVec4& color);
    void DrawGlyphCentered(ImVec2 center, const char* glyph, ImFont* font, const ImVec4& color);
}
