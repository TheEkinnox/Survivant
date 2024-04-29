#include "SurvivantRendering/Resources/TextureMetaData.h"

#include <SurvivantCore/Debug/Assertion.h>

using namespace SvCore::Serialization;
using namespace SvRendering::Enums;

namespace SvRendering::Resources
{
    bool TextureMetaData::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("wrap_u");
        if (!CHECK(p_writer.Uint(static_cast<uint8_t>(m_wrapModeU)), "Failed to write texture horizontal wrap mode"))
            return false;

        p_writer.Key("wrap_v");
        if (!CHECK(p_writer.Uint(static_cast<uint8_t>(m_wrapModeV)), "Failed to write texture vertical wrap mode"))
            return false;

        p_writer.Key("min_filter");
        if (!CHECK(p_writer.Uint(static_cast<uint8_t>(m_minFilter)), "Failed to write texture minification filter"))
            return false;

        p_writer.Key("mag_filter");
        if (!CHECK(p_writer.Uint(static_cast<uint8_t>(m_magFilter)), "Failed to write texture magnification filter"))
            return false;

        p_writer.Key("gen_mip");
        if (!CHECK(p_writer.Bool(m_generateMipmap), "Failed to write texture mipmap generation"))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to write texture meta data");
    }

    bool TextureMetaData::FromJson(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize texture meta data - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("wrap_u");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize texture horizontal wrap mode"))
            return false;

        m_wrapModeU = static_cast<ETextureWrapMode>(it->value.GetUint());

        it = p_json.FindMember("wrap_v");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize texture vertical wrap mode"))
            return false;

        m_wrapModeV = static_cast<ETextureWrapMode>(it->value.GetUint());

        it = p_json.FindMember("min_filter");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize texture minification filter"))
            return false;

        m_minFilter = static_cast<ETextureFilter>(it->value.GetUint());

        it = p_json.FindMember("mag_filter");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize texture magnification filter"))
            return false;

        m_magFilter = static_cast<ETextureFilter>(it->value.GetUint());

        it = p_json.FindMember("gen_mip");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsBool(), "Unable to deserialize texture mipmap generation"))
            return false;

        m_generateMipmap = it->value.GetBool();

        return true;
    }
}
