#pragma once
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace SvCore::Serialization
{
    using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;
    using JsonValue = rapidjson::Value;

    template <typename T>
    bool ToJson(const T& value, JsonWriter& writer) requires (!std::is_enum_v<T>);

    template <typename T>
    bool FromJson(T& value, const JsonValue& json) requires (!std::is_enum_v<T>);

    template <typename T>
    bool ToJson(T value, JsonWriter& writer) requires std::is_enum_v<T>;

    template <typename T>
    bool FromJson(T& out, const JsonValue& json) requires std::is_enum_v<T>;
}

#include "SurvivantCore/Serialization/Serializer.inl"
