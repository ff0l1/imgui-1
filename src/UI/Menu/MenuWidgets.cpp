#include "MenuWidgets.h"
#include "UI/Components/Layout.h"
#include "UI/Theme/Theme.h"

#include "imgui.h"
#include <Xinput.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace UI::Menu
{
        ImU32 Color(int r, int g, int b, int a)
        {
            return IM_COL32(r, g, b, a);
        }

        bool DrawToggle(const char* id, ImVec2 origin, bool& value, float animation)
        {
            constexpr ImVec2 size(38.0f, 22.0f);
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(origin);
            const bool clicked = ImGui::InvisibleButton("##toggle", size);
            const bool hovered = ImGui::IsItemHovered();
            ImGui::PopID();
            if (clicked)
                value = !value;

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            const int trackValue = static_cast<int>(42.0f + animation * 166.0f + (hovered ? 7.0f : 0.0f));
            const ImU32 track = Color(trackValue, trackValue + 2, trackValue + 7);
            drawList->AddRectFilled(origin, ImVec2(origin.x + size.x, origin.y + size.y), track, size.y * 0.5f);
            const float eased = animation * animation * (3.0f - 2.0f * animation);
            const float knobX = origin.x + 11.0f + eased * (size.x - 22.0f);
            drawList->AddCircleFilled(ImVec2(knobX, origin.y + 11.0f), 7.0f,
                animation > 0.5f ? Color(19, 21, 27) : Color(151, 154, 164), 18);
            return clicked;
        }

        bool DrawFpsSlider(const char* id, ImVec2 origin, float width, int& value)
        {
            constexpr float height = 26.0f;
            constexpr float endpointInset = 7.0f;
            const float trackWidth = width - endpointInset * 2.0f;
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(origin);
            ImGui::InvisibleButton("##slider", ImVec2(width, height));
            const bool hovered = ImGui::IsItemHovered();
            const bool active = ImGui::IsItemActive();
            bool changed = false;
            if (active)
            {
                const float normalized = (ImGui::GetIO().MousePos.x - origin.x - endpointInset) / trackWidth;
                const float clamped = normalized < 0.0f ? 0.0f : (normalized > 1.0f ? 1.0f : normalized);
                const int next = 30 + static_cast<int>(clamped * 970.0f + 0.5f);
                changed = next != value;
                value = next;
            }
            ImGui::PopID();

            const float normalized = static_cast<float>(value - 30) / 970.0f;
            const float centerY = origin.y + height * 0.5f;
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, centerY - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth, centerY + 2.0f), Color(42, 45, 54), 2.0f);
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, centerY - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth * normalized, centerY + 2.0f), Color(193, 197, 207), 2.0f);
            const float knobRadius = active ? 7.0f : (hovered ? 6.5f : 6.0f);
            drawList->AddCircleFilled(ImVec2(origin.x + endpointInset + trackWidth * normalized, centerY),
                knobRadius, Color(231, 233, 239), 20);
            return changed;
        }

        bool DrawIntRangeSlider(const char* id, ImVec2 origin, float width,
            int& value, int minimum, int maximum)
        {
            constexpr float height = 26.0f;
            constexpr float endpointInset = 7.0f;
            const float trackWidth = width - endpointInset * 2.0f;
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(origin);
            ImGui::InvisibleButton("##slider", ImVec2(width, height));
            const bool hovered = ImGui::IsItemHovered();
            const bool active = ImGui::IsItemActive();
            bool changed = false;
            if (active)
            {
                const float raw = (ImGui::GetIO().MousePos.x - origin.x - endpointInset) / trackWidth;
                const float t = (std::max)(0.0f, (std::min)(1.0f, raw));
                const int next = minimum + static_cast<int>(t * (maximum - minimum) + 0.5f);
                changed = next != value;
                value = next;
            }
            ImGui::PopID();
            const float t = static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum);
            const float y = origin.y + height * 0.5f;
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, y - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth, y + 2.0f),
                Color(42, 45, 54), 2.0f);
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, y - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth * t, y + 2.0f),
                Color(193, 197, 207), 2.0f);
            drawList->AddCircleFilled(ImVec2(origin.x + endpointInset + trackWidth * t, y),
                active ? 7.0f : (hovered ? 6.5f : 6.0f),
                Color(231, 233, 239), 20);
            return changed;
        }

        bool DrawFloatRangeSlider(const char* id, ImVec2 origin, float width,
            float& value, float minimum, float maximum)
        {
            constexpr float height = 26.0f;
            constexpr float endpointInset = 7.0f;
            const float trackWidth = width - endpointInset * 2.0f;
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(origin);
            ImGui::InvisibleButton("##slider", ImVec2(width, height));
            const bool hovered = ImGui::IsItemHovered();
            const bool active = ImGui::IsItemActive();
            if (active)
            {
                const float raw = (ImGui::GetIO().MousePos.x - origin.x - endpointInset) / trackWidth;
                const float t = (std::max)(0.0f, (std::min)(1.0f, raw));
                value = minimum + t * (maximum - minimum);
            }
            ImGui::PopID();
            const float t = (value - minimum) / (maximum - minimum);
            const float y = origin.y + height * 0.5f;
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, y - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth, y + 2.0f),
                Color(42, 45, 54), 2.0f);
            drawList->AddRectFilled(ImVec2(origin.x + endpointInset, y - 2.0f),
                ImVec2(origin.x + endpointInset + trackWidth * t, y + 2.0f),
                Color(193, 197, 207), 2.0f);
            drawList->AddCircleFilled(ImVec2(origin.x + endpointInset + trackWidth * t, y),
                active ? 7.0f : (hovered ? 6.5f : 6.0f),
                Color(231, 233, 239), 20);
            return active;
        }

        const char* GamepadButtonName(WORD button)
        {
            switch (button)
            {
            case XINPUT_GAMEPAD_A: return "Gamepad A";
            case XINPUT_GAMEPAD_B: return "Gamepad B";
            case XINPUT_GAMEPAD_X: return "Gamepad X";
            case XINPUT_GAMEPAD_Y: return "Gamepad Y";
            case XINPUT_GAMEPAD_LEFT_SHOULDER: return "Left bumper";
            case XINPUT_GAMEPAD_RIGHT_SHOULDER: return "Right bumper";
            case XINPUT_GAMEPAD_BACK: return "View";
            case XINPUT_GAMEPAD_START: return "Menu";
            case XINPUT_GAMEPAD_LEFT_THUMB: return "Left stick";
            case XINPUT_GAMEPAD_RIGHT_THUMB: return "Right stick";
            case XINPUT_GAMEPAD_DPAD_UP: return "D-pad up";
            case XINPUT_GAMEPAD_DPAD_DOWN: return "D-pad down";
            case XINPUT_GAMEPAD_DPAD_LEFT: return "D-pad left";
            case XINPUT_GAMEPAD_DPAD_RIGHT: return "D-pad right";
            default: return "Controller";
            }
        }

        WORD FirstGamepadButton(WORD buttons)
        {
            constexpr WORD order[] = {
                XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
                XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER,
                XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_START,
                XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
                XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
                XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT
            };
            for (WORD button : order)
            {
                if ((buttons & button) != 0)
                    return button;
            }
            return 0;
        }

        const char* VirtualKeyName(int virtualKey, char* buffer, size_t bufferSize)
        {
            switch (virtualKey)
            {
            case VK_LBUTTON: return "Mouse 1";
            case VK_RBUTTON: return "Mouse 2";
            case VK_MBUTTON: return "Mouse 3";
            case VK_XBUTTON1: return "Mouse 4";
            case VK_XBUTTON2: return "Mouse 5";
            case VK_CANCEL: return "Break";
            case VK_BACK: return "Backspace";
            case VK_TAB: return "Tab";
            case VK_CLEAR: return "Clear";
            case VK_RETURN: return "Enter";
            case VK_SHIFT: return "Shift";
            case VK_CONTROL: return "Ctrl";
            case VK_MENU: return "Alt";
            case VK_PAUSE: return "Pause";
            case VK_CAPITAL: return "Caps Lock";
            case VK_ESCAPE: return "Escape";
            case VK_SPACE: return "Space";
            case VK_PRIOR: return "Page Up";
            case VK_NEXT: return "Page Down";
            case VK_END: return "End";
            case VK_HOME: return "Home";
            case VK_LEFT: return "Left";
            case VK_UP: return "Up";
            case VK_RIGHT: return "Right";
            case VK_DOWN: return "Down";
            case VK_SELECT: return "Select";
            case VK_PRINT: return "Print";
            case VK_SNAPSHOT: return "Print Screen";
            case VK_INSERT: return "Insert";
            case VK_DELETE: return "Delete";
            case VK_HELP: return "Help";
            case VK_LWIN: return "Left Windows";
            case VK_RWIN: return "Right Windows";
            case VK_APPS: return "Menu Key";
            case VK_SLEEP: return "Sleep";
            case VK_MULTIPLY: return "Numpad *";
            case VK_ADD: return "Numpad +";
            case VK_SEPARATOR: return "Numpad Separator";
            case VK_SUBTRACT: return "Numpad -";
            case VK_DECIMAL: return "Numpad .";
            case VK_DIVIDE: return "Numpad /";
            case VK_NUMLOCK: return "Num Lock";
            case VK_SCROLL: return "Scroll Lock";
            case VK_LSHIFT: return "Left Shift";
            case VK_RSHIFT: return "Right Shift";
            case VK_LCONTROL: return "Left Ctrl";
            case VK_RCONTROL: return "Right Ctrl";
            case VK_LMENU: return "Left Alt";
            case VK_RMENU: return "Right Alt";
            case VK_BROWSER_BACK: return "Browser Back";
            case VK_BROWSER_FORWARD: return "Browser Forward";
            case VK_BROWSER_REFRESH: return "Browser Refresh";
            case VK_BROWSER_STOP: return "Browser Stop";
            case VK_BROWSER_SEARCH: return "Browser Search";
            case VK_BROWSER_FAVORITES: return "Browser Favorites";
            case VK_BROWSER_HOME: return "Browser Home";
            case VK_VOLUME_MUTE: return "Volume Mute";
            case VK_VOLUME_DOWN: return "Volume Down";
            case VK_VOLUME_UP: return "Volume Up";
            case VK_MEDIA_NEXT_TRACK: return "Media Next";
            case VK_MEDIA_PREV_TRACK: return "Media Previous";
            case VK_MEDIA_STOP: return "Media Stop";
            case VK_MEDIA_PLAY_PAUSE: return "Media Play/Pause";
            case VK_LAUNCH_MAIL: return "Launch Mail";
            case VK_LAUNCH_MEDIA_SELECT: return "Launch Media";
            case VK_LAUNCH_APP1: return "Launch App 1";
            case VK_LAUNCH_APP2: return "Launch App 2";
            case VK_OEM_1: return ";";
            case VK_OEM_PLUS: return "=";
            case VK_OEM_COMMA: return ",";
            case VK_OEM_MINUS: return "-";
            case VK_OEM_PERIOD: return ".";
            case VK_OEM_2: return "/";
            case VK_OEM_3: return "`";
            case VK_OEM_4: return "[";
            case VK_OEM_5: return "\\";
            case VK_OEM_6: return "]";
            case VK_OEM_7: return "'";
            case VK_OEM_8: return "OEM 8";
            case VK_OEM_102: return "OEM 102";
            default: break;
            }
            if ((virtualKey >= '0' && virtualKey <= '9') || (virtualKey >= 'A' && virtualKey <= 'Z'))
            {
                buffer[0] = static_cast<char>(virtualKey);
                buffer[1] = '\0';
                return buffer;
            }
            if (virtualKey >= VK_NUMPAD0 && virtualKey <= VK_NUMPAD9)
            {
                snprintf(buffer, bufferSize, "Numpad %d", virtualKey - VK_NUMPAD0);
                return buffer;
            }
            if (virtualKey >= VK_F1 && virtualKey <= VK_F24)
            {
                snprintf(buffer, bufferSize, "F%d", virtualKey - VK_F1 + 1);
                return buffer;
            }
            snprintf(buffer, bufferSize, "VK %02X", virtualKey);
            return buffer;
        }

        void DrawLabel(ImVec2 origin, const char* title, const char* detail)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            Components::DrawTextAt(origin, title, typography.BodyMedium, colors.TextPrimary);
            Components::DrawTextAt(ImVec2(origin.x, origin.y + 22.0f), detail, typography.Small, colors.TextSecondary);
        }

        void DrawDivider(ImVec2 min, ImVec2 max)
        {
            ImGui::GetWindowDrawList()->AddLine(min, max, Color(255, 255, 255, 13), 1.0f);
        }

        void DrawSectionCard(ImVec2 min, ImVec2 max, const char* title)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(min, max, Color(17, 20, 27), 14.0f);
            drawList->AddRect(min, max, Color(255, 255, 255, 10), 14.0f);
            Components::DrawTextAt(ImVec2(min.x + 14.0f, min.y + 11.0f), title,
                typography.BodyMedium, colors.TextPrimary);
            DrawDivider(ImVec2(min.x + 14.0f, min.y + 32.0f), ImVec2(max.x - 14.0f, min.y + 32.0f));
        }

        bool DrawCompactToggle(const char* id, ImVec2 origin, float width, const char* label,
            bool& value, bool enabled)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            const ImGuiID animationId = ImGui::GetID(id);
            ImGuiStorage* storage = ImGui::GetStateStorage();
            float animation = storage->GetFloat(animationId, value ? 1.0f : 0.0f);
            animation += ((value ? 1.0f : 0.0f) - animation)
                * (1.0f - std::exp(-ImGui::GetIO().DeltaTime * 14.0f));
            storage->SetFloat(animationId, animation);

            ImVec4 textColor = enabled ? colors.TextPrimary : colors.TextSecondary;
            if (!enabled)
                textColor.w *= 0.42f;
            Components::DrawTextAt(ImVec2(origin.x, origin.y + 3.0f), label, typography.Small, textColor);
            if (!enabled)
                ImGui::BeginDisabled();
            const bool clicked = DrawToggle(id, ImVec2(origin.x + width - 38.0f, origin.y), value, animation);
            if (!enabled)
                ImGui::EndDisabled();
            return clicked;
        }

        void DrawCompactIntSlider(const char* id, ImVec2 origin, float width, const char* label,
            int& value, int minimum, int maximum, const char* suffix, bool enabled)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImVec4 textColor = enabled ? colors.TextPrimary : colors.TextSecondary;
            if (!enabled)
                textColor.w *= 0.42f;
            char valueText[32];
            snprintf(valueText, sizeof(valueText), "%d%s", value, suffix);
            Components::DrawTextAt(origin, label, typography.Small, textColor);
            const float valueWidth = Components::TextWidth(valueText, typography.Small);
            Components::DrawTextAt(ImVec2(origin.x + width - valueWidth, origin.y), valueText,
                typography.Small, textColor);
            if (!enabled)
                ImGui::BeginDisabled();
            DrawIntRangeSlider(id, ImVec2(origin.x - 7.0f, origin.y + 18.0f),
                width + 14.0f, value, minimum, maximum);
            if (!enabled)
                ImGui::EndDisabled();
        }

        void DrawCompactFloatSlider(const char* id, ImVec2 origin, float width, const char* label,
            float& value, float minimum, float maximum, const char* format, bool enabled)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImVec4 textColor = enabled ? colors.TextPrimary : colors.TextSecondary;
            if (!enabled)
                textColor.w *= 0.42f;
            char valueText[32];
            snprintf(valueText, sizeof(valueText), format, value);
            Components::DrawTextAt(origin, label, typography.Small, textColor);
            const float valueWidth = Components::TextWidth(valueText, typography.Small);
            Components::DrawTextAt(ImVec2(origin.x + width - valueWidth, origin.y), valueText,
                typography.Small, textColor);
            if (!enabled)
                ImGui::BeginDisabled();
            DrawFloatRangeSlider(id, ImVec2(origin.x - 7.0f, origin.y + 18.0f),
                width + 14.0f, value, minimum, maximum);
            if (!enabled)
                ImGui::EndDisabled();
        }

        bool DrawDropdown(const char* id, ImVec2 origin, float width, const char* label,
            int& selected, const char* const* options, int optionCount, bool enabled)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec4 labelColor = enabled ? colors.TextPrimary : colors.TextSecondary;
            if (!enabled)
                labelColor.w *= 0.42f;
            Components::DrawTextAt(origin, label, typography.Small, labelColor);
            const ImVec2 fieldMin(origin.x + width - 154.0f, origin.y - 3.0f);
            const ImVec2 fieldMax(origin.x + width, origin.y + 27.0f);
            if (!enabled)
                ImGui::BeginDisabled();
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(fieldMin);
            const bool clicked = ImGui::InvisibleButton("##field", ImVec2(154.0f, 30.0f));
            const bool hovered = ImGui::IsItemHovered();
            const bool popupOpen = ImGui::IsPopupOpen("##options");
            if (clicked)
                ImGui::OpenPopup("##options");
            drawList->AddRectFilled(fieldMin, fieldMax,
                popupOpen ? Color(35, 39, 49) : (hovered && enabled ? Color(31, 35, 44) : Color(24, 27, 35)), 12.0f);
            drawList->AddRect(fieldMin, fieldMax,
                popupOpen ? Color(255, 255, 255, 28) : Color(255, 255, 255, enabled ? 13 : 6), 12.0f);
            Components::DrawTextAt(ImVec2(fieldMin.x + 11.0f, fieldMin.y + 7.0f), options[selected],
                typography.Small, labelColor);
            drawList->AddTriangleFilled(ImVec2(fieldMax.x - 17.0f, fieldMin.y + (popupOpen ? 16.0f : 11.0f)),
                ImVec2(fieldMax.x - 9.0f, fieldMin.y + (popupOpen ? 16.0f : 11.0f)),
                ImVec2(fieldMax.x - 13.0f, fieldMin.y + (popupOpen ? 11.0f : 16.0f)),
                Color(170, 174, 184, enabled ? 220 : 80));
            bool changed = false;
            ImGui::SetNextWindowPos(ImVec2(fieldMin.x, fieldMax.y + 7.0f));
            ImGui::SetNextWindowSize(ImVec2(156.0f, optionCount * 30.0f + 16.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 18.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7.0f, 7.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.065f, 0.075f, 0.10f, 0.98f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
            if (ImGui::BeginPopup("##options", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
            {
                ImDrawList* popupDrawList = ImGui::GetWindowDrawList();
                for (int index = 0; index < optionCount; ++index)
                {
                    const ImVec2 optionMin = ImGui::GetCursorScreenPos();
                    ImGui::SetCursorScreenPos(optionMin);
                    if (ImGui::InvisibleButton(options[index], ImVec2(142.0f, 28.0f)))
                    {
                        selected = index;
                        changed = true;
                        ImGui::CloseCurrentPopup();
                    }
                    const bool optionHovered = ImGui::IsItemHovered();
                    if (optionHovered || selected == index)
                        popupDrawList->AddRectFilled(optionMin, ImVec2(optionMin.x + 142.0f, optionMin.y + 28.0f),
                            selected == index ? Color(255, 255, 255, 18) : Color(255, 255, 255, 10), 11.0f);
                    Components::DrawTextAt(ImVec2(optionMin.x + 10.0f, optionMin.y + 5.0f), options[index],
                        typography.Small, selected == index ? colors.TextPrimary : colors.TextSecondary);
                    if (selected == index)
                        popupDrawList->AddCircleFilled(ImVec2(optionMin.x + 130.0f, optionMin.y + 14.0f), 2.5f,
                            Color(225, 228, 236), 12);
                    ImGui::SetCursorScreenPos(ImVec2(optionMin.x, optionMin.y + 30.0f));
                }
                ImGui::EndPopup();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);
            ImGui::PopID();
            if (!enabled)
                ImGui::EndDisabled();
            return changed;
        }

        bool DrawKeybindField(const char* id, ImVec2 origin, float width, const char* text, bool capturing)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(origin);
            const bool clicked = ImGui::InvisibleButton("##bind", ImVec2(width, 28.0f));
            const bool hovered = ImGui::IsItemHovered();
            ImGui::PopID();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(origin, ImVec2(origin.x + width, origin.y + 28.0f),
                capturing ? Color(38, 42, 52) : (hovered ? Color(31, 35, 44) : Color(24, 27, 35)), 9.0f);
            drawList->AddRect(origin, ImVec2(origin.x + width, origin.y + 28.0f),
                capturing ? Color(160, 166, 181, 90) : Color(255, 255, 255, 15), 9.0f);
            Components::DrawTextCentered(ImVec2(origin.x, origin.y + 6.0f), width, text,
                typography.Small, capturing ? colors.TextPrimary : colors.TextSecondary);
            return clicked;
        }

        bool DrawMultiDropdown(const char* id, ImVec2 origin, float width, const char* label,
            unsigned int& selected, const char* const* options, int optionCount)
        {
            char summary[64]{};
            int selectedCount = 0;
            for (int index = 0; index < optionCount; ++index)
            {
                if ((selected & (1u << index)) == 0)
                    continue;
                if (selectedCount == 0)
                    snprintf(summary, sizeof(summary), "%s", options[index]);
                ++selectedCount;
            }
            if (selectedCount == 0)
                snprintf(summary, sizeof(summary), "None");
            else if (selectedCount > 1)
                snprintf(summary, sizeof(summary), "%d bones", selectedCount);

            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            Components::DrawTextAt(origin, label, typography.Small, colors.TextPrimary);
            const ImVec2 fieldMin(origin.x + width - 154.0f, origin.y - 3.0f);
            const ImVec2 fieldMax(origin.x + width, origin.y + 27.0f);
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(fieldMin);
            if (ImGui::InvisibleButton("##field", ImVec2(154.0f, 30.0f)))
                ImGui::OpenPopup("##multi_options");
            const bool hovered = ImGui::IsItemHovered();
            const bool popupOpen = ImGui::IsPopupOpen("##multi_options");
            drawList->AddRectFilled(fieldMin, fieldMax,
                popupOpen ? Color(35, 39, 49) : (hovered ? Color(31, 35, 44) : Color(24, 27, 35)), 12.0f);
            drawList->AddRect(fieldMin, fieldMax, Color(255, 255, 255, popupOpen ? 28 : 13), 12.0f);
            Components::DrawTextAt(ImVec2(fieldMin.x + 11.0f, fieldMin.y + 7.0f), summary,
                typography.Small, colors.TextPrimary);
            ImGui::SetNextWindowPos(ImVec2(fieldMin.x, fieldMax.y + 7.0f));
            ImGui::SetNextWindowSize(ImVec2(156.0f, optionCount * 30.0f + 16.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 18.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7.0f, 7.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.065f, 0.075f, 0.10f, 0.98f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
            bool changed = false;
            if (ImGui::BeginPopup("##multi_options", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
            {
                ImDrawList* popupDrawList = ImGui::GetWindowDrawList();
                for (int index = 0; index < optionCount; ++index)
                {
                    const ImVec2 optionMin = ImGui::GetCursorScreenPos();
                    ImGui::SetCursorScreenPos(optionMin);
                    ImGui::PushID(index);
                    if (ImGui::InvisibleButton("##option", ImVec2(142.0f, 28.0f)))
                    {
                        const unsigned int next = selected ^ (1u << index);
                        if (next != 0)
                        {
                            selected = next;
                            changed = true;
                        }
                    }
                    ImGui::PopID();
                    const bool active = (selected & (1u << index)) != 0;
                    if (ImGui::IsItemHovered() || active)
                        popupDrawList->AddRectFilled(optionMin, ImVec2(optionMin.x + 142.0f, optionMin.y + 28.0f),
                            active ? Color(255, 255, 255, 18) : Color(255, 255, 255, 10), 11.0f);
                    Components::DrawTextAt(ImVec2(optionMin.x + 10.0f, optionMin.y + 5.0f), options[index],
                        typography.Small, active ? colors.TextPrimary : colors.TextSecondary);
                    const ImVec2 checkMin(optionMin.x + 119.0f, optionMin.y + 7.0f);
                    popupDrawList->AddRect(checkMin, ImVec2(checkMin.x + 14.0f, checkMin.y + 14.0f),
                        Color(255, 255, 255, active ? 85 : 24), 4.0f);
                    if (active)
                        popupDrawList->AddRectFilled(ImVec2(checkMin.x + 3.0f, checkMin.y + 3.0f),
                            ImVec2(checkMin.x + 11.0f, checkMin.y + 11.0f), Color(222, 225, 233), 2.0f);
                    ImGui::SetCursorScreenPos(ImVec2(optionMin.x, optionMin.y + 30.0f));
                }
                ImGui::EndPopup();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);
            ImGui::PopID();
            return changed;
        }

        void DrawColorRow(const char* id, ImVec2 origin, float width, const char* label, ImVec4& color)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            Components::DrawTextAt(ImVec2(origin.x, origin.y + 3.0f), label, typography.Small, colors.TextPrimary);
            const ImVec2 swatchMin(origin.x + width - 28.0f, origin.y);
            const ImVec2 swatchMax(swatchMin.x + 28.0f, swatchMin.y + 22.0f);
            ImGui::PushID(id);
            ImGui::SetCursorScreenPos(swatchMin);
            if (ImGui::InvisibleButton("##swatch", ImVec2(28.0f, 22.0f)))
                ImGui::OpenPopup("##picker");
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(swatchMin, swatchMax, ImGui::ColorConvertFloat4ToU32(color), 6.0f);
            drawList->AddRect(swatchMin, swatchMax, Color(255, 255, 255, 35), 6.0f);
            constexpr ImVec2 popupSize(194.0f, 226.0f);
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            const ImVec2 workMin = viewport->WorkPos;
            const ImVec2 workMax(viewport->WorkPos.x + viewport->WorkSize.x,
                viewport->WorkPos.y + viewport->WorkSize.y);
            float popupX = swatchMax.x - popupSize.x;
            float popupY = swatchMax.y + 6.0f;
            if (popupY + popupSize.y > workMax.y - 8.0f)
                popupY = swatchMin.y - popupSize.y - 6.0f;
            popupX = (std::max)(workMin.x + 8.0f, (std::min)(popupX, workMax.x - popupSize.x - 8.0f));
            popupY = (std::max)(workMin.y + 8.0f, (std::min)(popupY, workMax.y - popupSize.y - 8.0f));
            ImGui::SetNextWindowPos(ImVec2(popupX, popupY));
            ImGui::SetNextWindowSize(popupSize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 9.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.055f, 0.065f, 0.085f, 0.98f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.10f));
            if (ImGui::BeginPopup("##picker"))
            {
                Components::DrawTextAt(ImGui::GetCursorScreenPos(), label, typography.BodyMedium, colors.TextPrimary);
                ImGui::Dummy(ImVec2(1.0f, 20.0f));
                const ImVec4 presets[] = {
                    ImVec4(0.96f, 0.96f, 0.98f, 1.0f), ImVec4(0.36f, 0.82f, 1.0f, 1.0f),
                    ImVec4(0.50f, 0.91f, 0.63f, 1.0f), ImVec4(0.73f, 0.51f, 1.0f, 1.0f),
                    ImVec4(1.0f, 0.45f, 0.54f, 1.0f), ImVec4(1.0f, 0.72f, 0.32f, 1.0f)
                };
                ImDrawList* popupDrawList = ImGui::GetWindowDrawList();
                ImVec2 preset = ImGui::GetCursorScreenPos();
                for (int index = 0; index < 6; ++index)
                {
                    const ImVec2 min(preset.x + index * 28.0f, preset.y);
                    ImGui::SetCursorScreenPos(min);
                    ImGui::PushID(index);
                    if (ImGui::InvisibleButton("##preset", ImVec2(22.0f, 18.0f)))
                        color = presets[index];
                    ImGui::PopID();
                    popupDrawList->AddRectFilled(min, ImVec2(min.x + 22.0f, min.y + 18.0f),
                        ImGui::ColorConvertFloat4ToU32(presets[index]), 6.0f);
                    popupDrawList->AddRect(min, ImVec2(min.x + 22.0f, min.y + 18.0f),
                        Color(255, 255, 255, 28), 6.0f);
                }
                ImGui::SetCursorScreenPos(ImVec2(preset.x, preset.y + 25.0f));
                ImGui::SetNextItemWidth(174.0f);
                ImGui::ColorPicker3("##hsv", &color.x,
                    ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayHSV |
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview);
                const ImVec2 pickerMin = ImGui::GetItemRectMin();
                const ImVec2 pickerMax = ImGui::GetItemRectMax();
                popupDrawList->AddRect(pickerMin, pickerMax, Color(255, 255, 255, 28), 10.0f, 0, 1.0f);
                ImGui::EndPopup();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(4);
            ImGui::PopID();
        }

        void DrawStatusRow(ImVec2 origin, float width, const char* label, const char* value, bool active)
        {
            const ThemeColors& colors = Theme::Get().Colors();
            const ThemeTypography& typography = Theme::Get().Typography();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddCircleFilled(ImVec2(origin.x + 5.0f, origin.y + 8.0f), 4.0f,
                active ? Color(112, 185, 144) : Color(221, 145, 72), 16);
            Components::DrawTextAt(ImVec2(origin.x + 18.0f, origin.y), label,
                typography.Small, colors.TextSecondary);
            const float valueWidth = Components::TextWidth(value, typography.Small);
            Components::DrawTextAt(ImVec2(origin.x + width - valueWidth, origin.y), value,
                typography.Small, active ? colors.TextPrimary : ImVec4(0.87f, 0.57f, 0.28f, 1.0f));
        }
}
