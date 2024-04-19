#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const Radian& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        return CHECK(p_writer.Double(p_component.raw()), "Failed to write radian");
    }

    template <>
    bool ComponentRegistry::FromJson(Radian& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsNumber(), "Unable to read radian - Json value should be a number"))
            return false;

        p_out = Radian(p_json.GetFloat());
        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(
        const Degree& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        return CHECK(p_writer.Double(p_component.raw()), "Failed to write degrees");
    }

    template <>
    bool ComponentRegistry::FromJson(Degree& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsNumber(), "Unable to read degrees - Json value should be a number"))
            return false;

        p_out = Degree(p_json.GetFloat());
        return true;
    }
}
