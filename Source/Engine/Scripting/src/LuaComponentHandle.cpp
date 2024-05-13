#include "SurvivantScripting/LuaComponentHandle.h"

namespace SvScripting
{
    bool LuaComponentHandle::operator==(const LuaComponentHandle& p_other) const
    {
        return m_typeId == p_other.m_typeId && m_owner == p_other.m_owner;
    }

    LuaComponentHandle::operator bool() const
    {
        return m_owner && m_owner.GetScene()->GetStorage(m_typeId).Contains(m_owner);
    }
}

namespace SvCore::ECS
{
    using namespace SvScripting;
    using namespace Serialization;

    template <>
    bool ComponentRegistry::ToJson(const LuaComponentHandle& p_value, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        p_writer.StartObject();

        p_writer.Key("owner");
        if (!ToJson(p_value.m_owner, p_writer, p_toSerialized))
            return false;

        p_writer.Key("type");

        if (p_value.m_typeId == 0)
            return CHECK(p_writer.Null() && p_writer.EndObject(), "Failed to writer lua component handle");

        const ComponentRegistry& instance = GetInstance();

        if (!CHECK(instance.Contains(p_value.m_typeId), "Unable to serialize component handle"))
            return false;

        const std::string&        typeName       = instance.GetRegisteredTypeName(p_value.m_typeId);
        const rapidjson::SizeType typeNameLength = static_cast<rapidjson::SizeType>(typeName.size());

        return CHECK(p_writer.String(typeName.c_str(), typeNameLength), "Failed to write lua component handle's type")
            && CHECK(p_writer.EndObject(), "Failed to writer lua component handle");
    }

    template <>
    bool ComponentRegistry::FromJson(LuaComponentHandle& p_out, const JsonValue& p_json, Scene* p_scene)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize lua component handle - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("owner");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua component handle - Missing owner"))
            return false;

        if (!FromJson(p_out.m_owner, it->value, p_scene))
            return false;

        it = p_json.FindMember("type");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua component handle - Missing component type"))
            return false;

        if (it->value.IsNull())
        {
            p_out.m_typeId = 0;
            return true;
        }

        if (!CHECK(it->value.IsString(), "Unable to deserilize lua component handle type name - Json value should be a string"))
            return false;

        const std::string typeName(it->value.GetString(), it->value.GetStringLength());

        ComponentRegistry& instance = GetInstance();

        if (!CHECK(instance.Contains(typeName), "Unable to deserilize lua component handle - Unkown type \"%s\"", typeName.c_str()))
            return false;

        p_out.m_typeId = instance.GetTypeInfo(typeName).m_typeId;
        return true;
    }
}