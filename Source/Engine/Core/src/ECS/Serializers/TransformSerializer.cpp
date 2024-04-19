#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const Transform& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        p_writer.StartObject();

        p_writer.Key("position");
        std::string str = p_component.getPosition().string();
        p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        p_writer.Key("rotation");
        str = p_component.getRotation().string();
        p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        p_writer.Key("scale");
        str = p_component.getScale().string();
        p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));

        return p_writer.EndObject();
    }

    template <>
    bool ComponentRegistry::FromJson(Transform& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize transform - Invalid json object"))
            return false;

        Vector3    position, scale;
        Quaternion rotation;

        auto it = p_json.FindMember("position");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform position"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> position;
        }

        it = p_json.FindMember("rotation");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform rotation"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> rotation;
        }

        it = p_json.FindMember("scale");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize transform scale"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> scale;
        }

        p_out.setAll(position, rotation, scale);
        return true;
    }
}
