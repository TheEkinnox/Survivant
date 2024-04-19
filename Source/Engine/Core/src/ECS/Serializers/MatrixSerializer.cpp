#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const Matrix3& p_matrix, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_matrix.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Matrix3");
    }

    template <>
    bool ComponentRegistry::FromJson(Matrix3& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Matrix3 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(
        const Matrix4& p_matrix, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_matrix.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Matrix4");
    }

    template <>
    bool ComponentRegistry::FromJson(Matrix4& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Matrix4 - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }
}
