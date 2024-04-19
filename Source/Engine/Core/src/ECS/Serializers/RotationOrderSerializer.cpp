#include "SurvivantCore/ECS/Serializers/MathSerializers.h"

using namespace LibMath;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const ERotationOrder& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        return CHECK(p_writer.Uint(static_cast<unsigned>(p_component)), "Failed to write rotation order");
    }

    template <>
    bool ComponentRegistry::FromJson(ERotationOrder& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsUint(), "Unable to read rotation order - Json value should be an unsigned integer"))
            return false;

        p_out = static_cast<ERotationOrder>(p_json.GetUint());
        return true;
    }
}
