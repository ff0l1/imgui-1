#include "IconFont.h"

namespace UI::Icons
{
    const ImWchar* SymbolRanges()
    {
        static const ImWchar ranges[] = { 0xe15b, 0xe15b, 0xe5cd, 0xe5cd, 0xe9ba, 0xe9ba,
                                          0xea07, 0xea07, 0xefd6, 0xefd6, 0xf0d3, 0xf0d3,
                                          0xf720, 0xf720, 0 };
        return ranges;
    }

    const ImWchar* BrandRanges()
    {
        static const ImWchar ranges[] = { 0xf392, 0xf392, 0 };
        return ranges;
    }
}
