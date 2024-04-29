#pragma once
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/Serializer.h"

namespace SvCore::Serialization
{
    template <typename T>
    bool ToJson(const T& p_value, JsonWriter& p_writer) requires (!std::is_enum_v<T>)
    {
        constexpr bool hasToJson = requires
        {
            p_value.ToJson(p_writer);
        };

        if constexpr (hasToJson)
            return p_value.ToJson(p_writer);
        else
            return ASSUME(false, "Json serialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool FromJson(T& p_out, const JsonValue& p_json) requires (!std::is_enum_v<T>)
    {
        constexpr bool hasFromJson = requires
        {
            p_out.FromJson(p_json);
        };

        if constexpr (hasFromJson)
            return p_out.FromJson(p_json);
        else
            return ASSUME(false, "Json deserialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool ToJson(const T p_value, JsonWriter& p_writer) requires std::is_enum_v<T>
    {
        return CHECK(p_writer.Uint64(static_cast<uint64_t>(p_value)), "Failed to write \"%s\"", typeid(T).name());
    }

    template <typename T>
    bool FromJson(T& p_out, const JsonValue& p_json) requires std::is_enum_v<T>
    {
        if (!CHECK(p_json.IsUint64(), "Failed to read \"%s\"", typeid(T).name()))
            return false;

        p_out = static_cast<T>(p_json.GetUint64());
        return true;
    }
}
