#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const Vector2& vector, JsonWriter& writer)
    {
        const std::string str = vector.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector2");
    }

    template <>
    bool FromJson(Vector2& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Vector2 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }

    template <>
    bool ToJson(const Vector3& vector, JsonWriter& writer)
    {
        const std::string str = vector.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector3");
    }

    template <>
    bool FromJson(Vector3& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Vector3 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }

    template <>
    bool ToJson(const Vector4& vector, JsonWriter& writer)
    {
        const std::string str = vector.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector4");
    }

    template <>
    bool FromJson(Vector4& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Vector4 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }
}
