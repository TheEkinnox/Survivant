#pragma once
#include "SurvivantCore/Utility/EnumFlags.h"

namespace SvPhysics
{
    enum class EAxisLock : uint8_t
    {
        NONE       = 0,
        X_POSITION = 1 << 0,
        Y_POSITION = 1 << 1,
        Z_POSITION = 1 << 2,
        X_ROTATION = 1 << 3,
        Y_ROTATION = 1 << 4,
        Z_ROTATION = 1 << 5
    };

    SV_ENUM_FLAGS(EAxisLock, uint8_t)
}
