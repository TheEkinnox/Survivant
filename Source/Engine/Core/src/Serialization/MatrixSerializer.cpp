#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const Matrix3& matrix, JsonWriter& writer)
    {
        const std::string str = matrix.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Matrix3");
    }

    template <>
    bool FromJson(Matrix3& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Matrix3 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }

    template <>
    bool ToJson(const Matrix4& matrix, JsonWriter& writer)
    {
        const std::string str = matrix.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Matrix4");
    }

    template <>
    bool FromJson(Matrix4& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Matrix4 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }
}
