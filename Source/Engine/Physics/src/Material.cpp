#include "SurvivantPhysics/Material.h"

using namespace SvCore::Serialization;

namespace SvPhysics
{
    bool Material::Load(const std::string& p_fileName)
    {
        return FromJson(LoadJsonFile(p_fileName));
    }

    bool Material::Save(const std::string& p_fileName)
    {
        JsonStringBuffer buffer;
        JsonWriter       writer(buffer);

        if (!ToJson(writer) || !ASSUME(writer.IsComplete(), "Failed to save physics material - Generated json is incomplete"))
            return false;

        std::ofstream fs(p_fileName);

        if (!CHECK(fs.is_open(), "Unable to open physics material file at path \"%s\"", p_fileName.c_str()))
            return false;

        fs << std::string_view(buffer.GetString(), buffer.GetLength());

        return CHECK(!fs.bad(), "Failed to write physics material to \"%s\"", p_fileName.c_str());
    }

    bool Material::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("static_friction");
        if (!CHECK(p_writer.Double(m_staticFriction), "Failed to write physics material static friction"))
            return false;

        p_writer.Key("dynamic_friction");
        if (!CHECK(p_writer.Double(m_dynamicFriction), "Failed to write physics material dynamic friction"))
            return false;

        p_writer.Key("restitution");
        if (!CHECK(p_writer.Double(m_restitution), "Failed to write physics material restitution coefficient"))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize physics material");
    }

    bool Material::FromJson(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize physics material - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("static_friction");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize physics material - Missing static friction"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize physics material static friction - Json value should be a float"))
            return false;

        m_staticFriction = it->value.GetFloat();

        it = p_json.FindMember("dynamic_friction");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize physics material - Missing dynamic friction"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize physics material dynamic friction - Json value should be a float"))
            return false;

        m_dynamicFriction = it->value.GetFloat();

        it = p_json.FindMember("restitution");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize physics material - Missing restitution coefficient"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize physics restitution coefficient - Json value should be a float"))
            return false;

        m_restitution = it->value.GetFloat();

        return true;
    }
}
