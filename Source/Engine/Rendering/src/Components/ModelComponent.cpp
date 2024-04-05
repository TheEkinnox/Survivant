#include "SurvivantRendering/Components/ModelComponent.h"

using namespace SvRendering::Components;

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson<ModelComponent>(
        const ModelComponent& p_model, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        p_writer.StartObject();

        p_writer.Key("model");

        if (!p_model.m_model.ToJson(p_writer))
            return false;

        p_writer.Key("material");

        if (!p_model.m_material.ToJson(p_writer))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize model component");
    }

    template <>
    bool ComponentRegistry::FromJson<ModelComponent>(ModelComponent& p_out, const rapidjson::Value& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Failed to deserialize model component - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("model");

        if (!CHECK(it != p_json.MemberEnd(), "Failed to deserialize model component - Missing model"))
            return false;

        if (!p_out.m_model.FromJson(it->value))
            return false;

        it = p_json.FindMember("material");

        if (!CHECK(it != p_json.MemberEnd(), "Failed to deserialize model component - Missing material"))
            return false;

        return p_out.m_material.FromJson(it->value);
    }
}
