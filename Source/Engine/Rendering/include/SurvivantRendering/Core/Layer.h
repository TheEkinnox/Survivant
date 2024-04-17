#pragma once
#include <cstdint>

namespace SvRendering::Core
{
    using LayerMask = uint32_t;

    namespace Layer
    {
        constexpr LayerMask NONE = 0;
        constexpr LayerMask ALL  = ~static_cast<LayerMask>(0);
    };
}