#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include <string>

namespace SvCore::ECS
{
    struct TagComponent
    {
        std::string m_tag;
    };
}

namespace SvCore::Serialization
{
    template <>
    bool Serialization::ToJson(const ECS::TagComponent&, JsonWriter&);

    template <>
    bool Serialization::FromJson(ECS::TagComponent&, const JsonValue&);
}
