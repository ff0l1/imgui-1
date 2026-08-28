#include "RenderOpacity.h"

#include <vector>

namespace UI
{
    namespace
    {
        std::vector<float>& Stack()
        {
            static std::vector<float> stack{ 1.0f };
            return stack;
        }
    }

    float RenderOpacity::Current()
    {
        return Stack().back();
    }

    void RenderOpacity::Push(float opacity)
    {
        Stack().push_back(Stack().back() * opacity);
    }

    void RenderOpacity::Pop()
    {
        if (Stack().size() > 1)
            Stack().pop_back();
    }

    ImVec4 ApplyOpacity(const ImVec4& color)
    {
        ImVec4 result = color;
        result.w *= RenderOpacity::Current();
        return result;
    }
}
