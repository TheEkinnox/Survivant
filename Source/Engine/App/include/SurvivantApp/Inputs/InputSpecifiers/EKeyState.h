#pragma once
#include <cstdint>

namespace SvApp
{
    enum class EKeyState : uint8_t
    {
        RELEASED = 0,
        PRESSED  = 1,
        REPEATED = 2
    };
}
