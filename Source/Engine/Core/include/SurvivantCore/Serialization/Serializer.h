#pragma once
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>

namespace SvCore::Serialization
{
    using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;
    using JsonOStream = rapidjson::OStreamWrapper;
    using JsonFileWriter = rapidjson::Writer<JsonOStream>;
    using JsonDocument = rapidjson::Document;
    using JsonValue = rapidjson::Value;

    /**
     * \brief Loads the json document at the given path
     * \param p_path The json file's path
     * \return The loaded json document
     */
    JsonDocument LoadJsonFile(const std::string& p_path);

    template <typename T>
    bool ToJson(const T& p_value, JsonWriter& p_writer) requires (!std::is_enum_v<T>);

    template <typename T>
    bool FromJson(T& p_out, const JsonValue& p_json) requires (!std::is_enum_v<T>);

    template <typename T>
    bool ToJson(T p_value, JsonWriter& p_writer) requires std::is_enum_v<T>;

    template <typename T>
    bool FromJson(T& p_out, const JsonValue& p_json) requires std::is_enum_v<T>;
}

#include "SurvivantCore/Serialization/Serializer.inl"
