#pragma once

#include "imgui.h"

#define ICON_CLOSE      "\xee\x97\x8d"
#define ICON_MINIMIZE   "\xee\x85\x9b"
#define ICON_GLOBE      "\xee\xa8\x87"
#define ICON_PRODUCTS   "\xef\x9c\xa0"
#define ICON_ACCOUNT    "\xef\x83\x93"
#define ICON_LOGOUT     "\xee\xa6\xba"
#define ICON_SCHEDULE   "\xee\xbf\x96"
#define ICON_DISCORD    "\xef\x8e\x92"

namespace UI::Icons
{
    const ImWchar* SymbolRanges();
    const ImWchar* BrandRanges();
}
