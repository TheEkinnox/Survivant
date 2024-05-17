#pragma once
#include <cstdint>

namespace SvPhysics
{
    /**
     * \brief The supported collision detection modes
     */
    enum class ECollisionDetectionMode : uint8_t
    {
        DISCRETE,
        CONTINUOUS,
        CONTINUOUS_SPECULATIVE
    };
}
