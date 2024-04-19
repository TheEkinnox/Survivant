#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

#include <Vector.h>

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const Vector2& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_component.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector2");
    }

    template <>
    bool ComponentRegistry::FromJson(Vector2& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Vector2 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(
        const Vector3& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_component.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector3");
    }

    template <>
    bool ComponentRegistry::FromJson(Vector3& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Vector3 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(
        const Vector4& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_component.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Vector4");
    }

    template <>
    bool ComponentRegistry::FromJson(Vector4& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Vector4 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }
}
