#pragma once
#include <SurvivantCore/Debug/Assertion.h>

#include <cstdint>

namespace SvRendering::Enums
{
    /**
     * \brief Supported camera projection types
     */
    enum class EProjectionType : uint8_t
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    /**
     * \brief Gets the given projection type's string representation
     * \param p_type The projection type to stringify
     * \return The given type's string representation
     */
    inline std::string ProjectionTypeToString(const EProjectionType p_type)
    {
        switch (p_type)
        {
        case EProjectionType::PERSPECTIVE:
            return "perspective";
        case EProjectionType::ORTHOGRAPHIC:
            return "orthographic";
        default:
            ASSERT(false, "Unsupported projection type");
            return {};
        }
    }

    /**
     * \brief Gets the projection type represented by the given string
     * \param p_type The type string to convert
     * \return The type corresponding to the given string
     */
    inline EProjectionType StringToProjectionType(std::string p_type)
    {
        SvCore::Utility::ToLowerInPlace(p_type);

        if (p_type == "perspective")
            return EProjectionType::PERSPECTIVE;

        if (p_type == "orthographic")
            return EProjectionType::ORTHOGRAPHIC;

        ASSERT(false, "Unsupported projection type");
        return EProjectionType::PERSPECTIVE;
    }
}
