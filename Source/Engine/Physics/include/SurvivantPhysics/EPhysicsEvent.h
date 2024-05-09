#pragma once
#include <cstdint>

namespace SvPhysics
{
    /**
     * \brief The supported physics event types
     */
    enum class EPhysicsEvent : uint8_t
    {
        NONE,
        ENTER,
        STAY,
        EXIT
    };
}
