#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const Quaternion& quaternion, JsonWriter& writer)
    {
        const std::string str = quaternion.string();
        return CHECK(writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Quaternion");
    }

    template <>
    bool FromJson(Quaternion& out, const JsonValue& json)
    {
        if (!CHECK(json.IsString(), "Unable to deserialize Quaternion - Invalid json value"))
            return false;

        std::istringstream iss(std::string(json.GetString(), json.GetStringLength()));
        iss >> out;

        return true;
    }
}
