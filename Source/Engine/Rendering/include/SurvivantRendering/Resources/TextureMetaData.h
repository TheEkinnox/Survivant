#pragma once
#include "SurvivantRendering/Enums/ETextureFilter.h"
#include "SurvivantRendering/Enums/ETextureWrapMode.h"

#include <SurvivantCore/Serialization/Serializer.h>

namespace SvRendering::Resources
{
    struct TextureMetaData
    {
        static constexpr uint8_t WRAP_MODE_BITS = 2;
        static constexpr uint8_t FILTER_BITS    = 3;
        static constexpr uint8_t BOOL_BITS      = 1;

        Enums::ETextureWrapMode m_wrapModeU      : WRAP_MODE_BITS = Enums::ETextureWrapMode::REPEAT;
        Enums::ETextureWrapMode m_wrapModeV      : WRAP_MODE_BITS = Enums::ETextureWrapMode::REPEAT;
        Enums::ETextureFilter   m_minFilter      : FILTER_BITS    = Enums::ETextureFilter::LINEAR;
        Enums::ETextureFilter   m_magFilter      : FILTER_BITS    = Enums::ETextureFilter::LINEAR;
        bool                    m_generateMipmap : BOOL_BITS      = true;

        /**
         * \brief Serializes the texture to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise.
         */
        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the texture from json
         * \param p_json The input json value
         * \return True on success. False otherwise.
         */
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);
    };
}
