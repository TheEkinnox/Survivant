#include "SurvivantScripting/LuaScriptHandle.h"

#include "SurvivantScripting/LuaScriptList.h"

using namespace SvCore::ECS;
using namespace SvCore::Serialization;

namespace SvScripting
{
    bool LuaScriptHandle::operator<(const LuaScriptHandle& p_other) const
    {
        return (m_script && p_other.m_script && m_script->GetExecutionOrder() < p_other.m_script->GetExecutionOrder())
            || m_name.compare(p_other.m_name) < 0
            || m_script.GetPath().compare(p_other.m_script.GetPath()) < 0
            || m_owner.GetEntity() < p_other.m_owner.GetEntity()
            || std::less<Scene*>{}(m_owner.GetScene(), p_other.m_owner.GetScene());
    }

    bool LuaScriptHandle::operator==(const LuaScriptHandle& p_other) const
    {
        return m_owner == p_other.m_owner && m_script == p_other.m_script;
    }

    LuaScriptHandle::operator bool() const
    {
        if (!m_owner || !m_script)
            return false;

        const LuaScriptList* scripts = m_owner.Get<LuaScriptList>();
        return scripts->Contains(m_name);
    }
}

namespace SvCore::ECS
{
    using namespace SvScripting;

    template <>
    bool ComponentRegistry::ToJson(const LuaScriptHandle& p_value, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        p_writer.StartObject();

        p_writer.Key("owner");
        if (!ToJson(p_value.m_owner, p_writer, p_toSerialized))
            return false;

        p_writer.Key("script");
        if (!p_value.m_script.ToJson(p_writer))
            return false;

        p_writer.Key("table");
        if (!ToJson(p_value.m_table, p_writer, p_toSerialized))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to write lua script handle");
    }

    template <>
    bool ComponentRegistry::FromJson(LuaScriptHandle& p_out, const JsonValue& p_json, Scene* p_scene)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize lua script handle - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("owner");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua script handle - Missing owner"))
            return false;

        if (!FromJson(p_out.m_owner, it->value, p_scene))
            return false;

        it = p_json.FindMember("script");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua script handle - Missing script"))
            return false;

        if (!p_out.m_script.FromJson(it->value))
            return false;

        it = p_json.FindMember("table");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua script handle - Missing table"))
            return false;

        return FromJson(p_out.m_table, it->value, p_scene);
    }
}
