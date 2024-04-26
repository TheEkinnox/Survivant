#include "SurvivantCore/ECS/Components/TagComponent.h"

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const ECS::TagComponent& p_value, JsonWriter& p_writer)
    {
        return CHECK(p_writer.String(p_value.m_tag.c_str(), static_cast<rapidjson::SizeType>(p_value.m_tag.size())));
    }

    template <>
    bool FromJson(ECS::TagComponent& p_out, const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize tag - Json value should be a string"))
            return false;

        p_out.m_tag = p_json.GetString();

        return true;
    }
}
