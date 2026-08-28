#include "ShellUtils.h"

#include <Windows.h>
#include <shellapi.h>

namespace Utilities
{
    void OpenURL(const std::string& url)
    {
        ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}
