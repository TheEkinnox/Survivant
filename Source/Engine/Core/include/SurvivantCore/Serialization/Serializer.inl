#pragma once
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/Serializer.h"

namespace SvCore::Serialization
{
    template <typename T>
    bool ToJson(const T& value, JsonWriter& writer) requires (!std::is_enum_v<T>)
    {
        constexpr bool hasToJson = requires
        {
            value.ToJson(writer);
        };

        if constexpr (hasToJson)
            return value.ToJson(writer);
        else
            return ASSUME(false, "Json serialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool FromJson(T& out, const JsonValue& json) requires (!std::is_enum_v<T>)
    {
        constexpr bool hasFromJson = requires
        {
            out.FromJson(json);
        };

        if constexpr (hasFromJson)
            return out.FromJson(json);
        else
            return ASSUME(false, "Json deserialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool ToJson(const T value, JsonWriter& writer) requires std::is_enum_v<T>
    {
        return CHECK(writer.Uint64(static_cast<uint64_t>(value)), "Failed to write \"%s\"", typeid(T).name());
    }

    template <typename T>
    bool FromJson(T& out, const JsonValue& json) requires std::is_enum_v<T>
    {
        if (!CHECK(json.IsUint64(), "Failed to read \"%s\"", typeid(T).name()))
            return false;

        out = static_cast<T>(json.GetUint64());
        return true;
    }
}
