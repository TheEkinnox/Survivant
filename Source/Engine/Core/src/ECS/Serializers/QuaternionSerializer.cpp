#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const Quaternion& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        const std::string str = p_component.string();
        return CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())), "Failed to serialize Quaternion");
    }

    template <>
    bool ComponentRegistry::FromJson(Quaternion& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsString(), "Unable to deserialize Quaternion - Invalid json value"))
            return false;

        std::istringstream iss(std::string(p_json.GetString(), p_json.GetStringLength()));
        iss >> p_out;

        return true;
    }
}
