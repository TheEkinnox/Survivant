#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include <string>

namespace SvCore::ECS
{
    struct TagComponent
    {
        std::string m_tag;
    };

    template <>
    bool ComponentRegistry::ToJson(const TagComponent&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(TagComponent&, const rapidjson::Value&, Scene*);
}
