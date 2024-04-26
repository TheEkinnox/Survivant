#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const Transform& transform, JsonWriter& writer)
    {
        writer.StartObject();

        writer.Key("position");
        std::string str = transform.getPosition().string();
        writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        writer.Key("rotation");
        str = transform.getRotation().string();
        writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        writer.Key("scale");
        str = transform.getScale().string();
        writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        return writer.EndObject();
    }

    template <>
    bool FromJson(Transform& transform, const JsonValue& json)
    {
        if (!CHECK(json.IsObject(), "Unable to deserialize transform - Invalid json object"))
            return false;

        Vector3    position, scale;
        Quaternion rotation;

        auto it = json.FindMember("position");
        if (!CHECK(it != json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform position"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> position;
        }

        it = json.FindMember("rotation");
        if (!CHECK(it != json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform rotation"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> rotation;
        }

        it = json.FindMember("scale");
        if (!CHECK(it != json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform scale"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> scale;
        }

        transform.setAll(position, rotation, scale);
        return true;
    }
}
