#pragma once

#include "imgui.h"

#include <Windows.h>
#include <cstddef>

namespace UI::Menu
{
    constexpr float kOuterMargin = 24.0f;
    constexpr float kMenuWidth = 940.0f;
    constexpr float kMenuHeight = 620.0f;
    constexpr float kPanelGap = 14.0f;
    constexpr float kTopBarHeight = 58.0f;
    constexpr float kLeftPanelWidth = kTopBarHeight;
    constexpr float kTopBarRadius = 20.0f;
    constexpr float kPanelRadius = 20.0f;
    constexpr float kBrandPadding = 22.0f;

    inline const ImVec4 kTopSurface(18.0f / 255.0f, 20.0f / 255.0f, 27.0f / 255.0f, 1.0f);
    inline const ImVec4 kRailSurface(16.0f / 255.0f, 18.0f / 255.0f, 24.0f / 255.0f, 1.0f);
    inline const ImVec4 kMainSurface(12.0f / 255.0f, 14.0f / 255.0f, 19.0f / 255.0f, 1.0f);

    ImU32 Color(int r, int g, int b, int a = 255);
    bool DrawToggle(const char* id, ImVec2 origin, bool& value, float animation);
    bool DrawFpsSlider(const char* id, ImVec2 origin, float width, int& value);
    bool DrawIntRangeSlider(const char* id, ImVec2 origin, float width, int& value, int minimum, int maximum);
    bool DrawFloatRangeSlider(const char* id, ImVec2 origin, float width, float& value, float minimum, float maximum);
    const char* GamepadButtonName(WORD button);
    WORD FirstGamepadButton(WORD buttons);
    const char* VirtualKeyName(int virtualKey, char* buffer, size_t bufferSize);
    void DrawLabel(ImVec2 origin, const char* title, const char* detail);
    void DrawDivider(ImVec2 min, ImVec2 max);
    void DrawSectionCard(ImVec2 min, ImVec2 max, const char* title);
    bool DrawCompactToggle(const char* id, ImVec2 origin, float width, const char* label, bool& value, bool enabled = true);
    void DrawCompactIntSlider(const char* id, ImVec2 origin, float width, const char* label,
        int& value, int minimum, int maximum, const char* suffix, bool enabled = true);
    void DrawCompactFloatSlider(const char* id, ImVec2 origin, float width, const char* label,
        float& value, float minimum, float maximum, const char* format, bool enabled = true);
    bool DrawDropdown(const char* id, ImVec2 origin, float width, const char* label,
        int& selected, const char* const* options, int optionCount, bool enabled = true);
    bool DrawKeybindField(const char* id, ImVec2 origin, float width, const char* text, bool capturing);
    bool DrawMultiDropdown(const char* id, ImVec2 origin, float width, const char* label,
        unsigned int& selected, const char* const* options, int optionCount);
    void DrawColorRow(const char* id, ImVec2 origin, float width, const char* label, ImVec4& color);
    void DrawStatusRow(ImVec2 origin, float width, const char* label, const char* value, bool active);
}
