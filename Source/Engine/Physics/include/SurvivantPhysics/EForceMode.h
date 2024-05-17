#pragma once
#include <cstdint>

namespace SvPhysics
{
    /**
     * \brief The supported force modes
     */
    enum class EForceMode : uint8_t
    {
        FORCE,
        IMPULSE,
        ACCELERATION,
        VELOCITY_CHANGE
    };
}
