#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const BoundingBox& p_box, JsonWriter& p_writer)
    {
        p_writer.StartObject();

        p_writer.Key("min");
        if (!ToJson(p_box.m_min, p_writer))
            return false;

        p_writer.Key("max");
        if (!ToJson(p_box.m_max, p_writer))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize bounding box");
    }

    template <>
    bool FromJson(BoundingBox& p_out, const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize bounding box - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("min");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize bounding box - Missing min"))
            return false;

        if (!FromJson(p_out.m_min, it->value))
            return false;

        it = p_json.FindMember("max");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize bounding box - Missing max"))
            return false;

        return FromJson(p_out.m_max, it->value);
    }

    template <>
    bool ToJson(const BoundingSphere& p_sphere, JsonWriter& p_writer)
    {
        p_writer.StartObject();

        p_writer.Key("center");
        if (!ToJson(p_sphere.m_center, p_writer))
            return false;

        p_writer.Key("radius");
        if (!CHECK(p_writer.Double(p_sphere.m_radius), "Failed to serialize bounding sphere radius"))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize bounding sphere");
    }

    template <>
    bool FromJson(BoundingSphere& p_out, const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize bounding sphere - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("center");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize bounding sphere - Missing center"))
            return false;

        if (!FromJson(p_out.m_center, it->value))
            return false;

        it = p_json.FindMember("radius");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize bounding sphere - Missing radius"))
            return false;

        if (!CHECK(it->value.Is<float>(), "Unable to deserialize bounding sphere radius - Json value should be a float"))
            return false;

        p_out.m_radius = it->value.Get<float>();

        return true;
    }
}
