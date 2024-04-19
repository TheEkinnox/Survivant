#include "SurvivantCore/ECS/Components/TagComponent.h"

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const TagComponent& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        return CHECK(p_writer.String(p_component.m_tag.c_str(), static_cast<rapidjson::SizeType>(p_component.m_tag.size())));
    }

    template <>
    bool ComponentRegistry::FromJson(TagComponent& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize tag - Json value should be a string"))
            return false;

        p_out.m_tag = p_json.GetString();

        return true;
    }
}
