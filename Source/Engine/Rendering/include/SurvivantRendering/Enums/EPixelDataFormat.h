#pragma once
#include <cstdint>

namespace SvRendering::Enums
{
    /**
     * \breif Supported pixel data formats
     */
    enum class EPixelDataFormat : uint8_t
    {
        STENCIL_INDEX,
        DEPTH_COMPONENT,
        DEPTH_STENCIL,
        RED,
        RED_INT,
        RED_INT_32,
        RED_UINT_32,
        GREEN,
        BLUE,
        ALPHA,
        RG,
        RGB,
        BGR,
        RGBA,
        BGRA
    };
}
