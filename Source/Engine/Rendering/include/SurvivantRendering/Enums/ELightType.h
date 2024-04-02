#pragma once
#include <SurvivantCore/Debug/Assertion.h>

#include <cstdint>

namespace SvRendering::Enums
{
    /**
     * \brief Supported light types
     */
    enum class ELightType : uint8_t
    {
        AMBIENT,
        DIRECTIONAL,
        POINT,
        SPOT
    };

    /**
     * \brief Gets the given light type's string representation
     * \param p_type The light type to stringify
     * \return The given type's string representation
     */
    inline std::string LightTypeToString(const ELightType p_type)
    {
        switch (p_type)
        {
        case ELightType::AMBIENT:
            return "ambient";
        case ELightType::DIRECTIONAL:
            return "directional";
        case ELightType::POINT:
            return "point";
        case ELightType::SPOT:
            return "spot";
        default:
            ASSERT(false, "Unsupported light type");
            return {};
        }
    }

    /**
     * \brief Gets the light type represented by the given string
     * \param p_type The type string to convert
     * \return The type corresponding to the given string
     */
    inline ELightType StringToLightType(std::string p_type)
    {
        SvCore::Utility::ToLowerInPlace(p_type);

        if (p_type == "ambient")
            return ELightType::AMBIENT;

        if (p_type == "directional")
            return ELightType::DIRECTIONAL;

        if (p_type == "point")
            return ELightType::POINT;

        if (p_type == "spot")
            return ELightType::SPOT;

        ASSERT(false, "Unsupported light type");
        return ELightType::AMBIENT;
    }
}
