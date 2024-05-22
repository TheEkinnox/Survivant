#include "SurvivantRendering/Components/ModelComponent.h"

using namespace SvRendering::Components;
using namespace SvRendering::Core;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const ModelComponent& p_model, JsonWriter& p_writer)
    {
        p_writer.StartObject();

        p_writer.Key("model");

        if (!p_model.m_model.ToJson(p_writer))
            return false;

        p_writer.Key("material");

        if (!p_model.m_material.ToJson(p_writer))
            return false;

        p_writer.Key("layers");

        if (!CHECK(p_writer.Uint64(p_model.m_layerMask), "Failed to serialize model component layer mask"))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize model component");
    }

    template <>
    bool FromJson(ModelComponent& p_out, const JsonValue& p_json)
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

        it = p_json.FindMember("layers");

        if (it != p_json.MemberEnd())
        {
            if (!CHECK(it->value.Is<LayerMask>(),
                    "Unable to deserialize camera component layer mask - Json value should be a uint"))
                return false;

            p_out.m_layerMask = it->value.Get<LayerMask>();
        }
        else
        {
            p_out.m_layerMask = Layer::ALL;
        }

        return p_out.m_material.FromJson(it->value);
    }
}
