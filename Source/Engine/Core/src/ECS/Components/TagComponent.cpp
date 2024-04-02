#include "SurvivantCore/ECS/Components/TagComponent.h"

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson<TagComponent>(
        const TagComponent& tag, rapidjson::Writer<rapidjson::StringBuffer>& writer, const EntitiesMap&)
    {
        return CHECK(writer.String(tag.m_tag.c_str(), static_cast<rapidjson::SizeType>(tag.m_tag.size())));
    }

    template <>
    bool ComponentRegistry::FromJson<TagComponent>(TagComponent& out, const rapidjson::Value& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize tag - Json value should be a string"))
            return false;

        out.m_tag = json.GetString();

        return true;
    }
}
