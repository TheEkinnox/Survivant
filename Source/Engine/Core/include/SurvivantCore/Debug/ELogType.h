#pragma once
#include <cstdint>

namespace SvCore::Debug
{
    enum class ELogType : uint8_t
    {
        DEFAULT_LOG = 1 << 0,
        DEBUG_LOG   = 1 << 1,
        WARNING_LOG = 1 << 2,
        ERROR_LOG   = 1 << 3,
        // COMMAND_LOG = 1 << 4
    };
}
