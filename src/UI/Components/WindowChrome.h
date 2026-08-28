#pragma once

#include "IconButton.h"
#include <Windows.h>
#include <memory>
#include <string>

namespace Window { class Win32Window; }

namespace UI::Components
{
    class WindowChrome
    {
    public:
        explicit WindowChrome(Window::Win32Window& window);

        void Initialize();
        void SetTitle(std::string title) { m_Title = std::move(title); }

        void Update(float deltaSeconds);
        void Draw(ImVec2 windowSize);
        void DrawInHeader(ImVec2 headerMin, ImVec2 headerMax);

    private:
        void UpdateDragging(ImVec2 dragMin, ImVec2 dragMax);
        void DrawTitle();

        Window::Win32Window& m_Window;
        std::unique_ptr<IconButton> m_MinimizeButton;
        std::unique_ptr<IconButton> m_CloseButton;
        std::string m_Title;
        bool m_Dragging;
        POINT m_DragStartCursor;
        POINT m_DragStartWindow;
    };
}
