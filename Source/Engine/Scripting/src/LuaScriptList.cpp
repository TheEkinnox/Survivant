#include "SurvivantScripting/LuaScriptList.h"

#include "SurvivantScripting/LuaContext.h"
#include "SurvivantScripting/LuaTypeRegistry.h"

#include <ranges>

using namespace SvCore::Serialization;

namespace SvScripting
{
    using namespace SvCore::ECS;

    LuaScriptList::~LuaScriptList()
    {
        Clear();
    }

    LuaScriptList& LuaScriptList::operator=(const LuaScriptList& p_other)
    {
        if (this == &p_other)
            return *this;

        Clear();

        m_owner   = p_other.m_owner;
        m_scripts = p_other.m_scripts;

        return *this;
    }

    LuaScriptList& LuaScriptList::operator=(LuaScriptList&& p_other) noexcept
    {
        if (this == &p_other)
            return *this;

        Clear();

        m_owner   = p_other.m_owner;
        m_scripts = std::move(p_other.m_scripts);

        return *this;
    }

    bool LuaScriptList::Contains(const std::string& p_script) const
    {
        return m_scripts.contains(LuaContext::GetModuleName(p_script));
    }

    LuaScriptHandle LuaScriptList::Get(const std::string& p_script) const
    {
        return LuaContext::GetInstance().GetScript(p_script, m_owner);
    }

    LuaScriptHandle LuaScriptList::Add(std::string p_script, const sol::table& p_hint)
    {
        p_script = LuaContext::GetModuleName(p_script);

        if (p_script.empty())
            return {};

        if (!CHECK(!m_scripts.contains(p_script), "Attempted to add script \"%s\" to entity \"%s\" more than once",
                p_script.c_str(), m_owner.GetEntity().GetString().c_str()))
            return {};

        LuaScriptHandle handle = LuaContext::GetInstance().AddScript(p_script, m_owner, p_hint);

        if (!handle.m_table.valid())
            return {};

        // Note: Re-fetch is necessary to avoid issues when entities are created during script initialization
        handle.m_owner.Get<LuaScriptList>()->m_scripts[p_script] = handle.m_table;
        return handle;
    }

    void LuaScriptList::Remove(const std::string& p_script)
    {
        LuaContext::GetInstance().RemoveScript(p_script, m_owner);
        m_scripts.erase(LuaContext::GetModuleName(p_script));
    }

    void LuaScriptList::Clear()
    {
        LuaContext& context = LuaContext::GetInstance();

        for (const auto& script : m_scripts | std::views::keys)
            context.RemoveScript(script, m_owner);

        m_scripts.clear();
    }

    sol::optional<sol::object> LuaObjectFromJson(lua_State* p_luaState, const JsonValue& p_json, Scene* p_scene)
    {
        if (!CHECK(p_luaState, "Unable to deserialize lua object - No lua state"))
            return sol::nullopt;

        auto it = p_json.FindMember("type");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsInt(), "Unable to deserialize lua object - Type should be an int"))
            return sol::nullopt;

        const sol::type objectType = static_cast<sol::type>(it->value.GetInt());

        it = p_json.FindMember("data");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize lua object - Missing value"))
            return sol::nullopt;

        switch (objectType)
        {
        case sol::type::lua_nil:
        {
            if (!CHECK(it->value.IsNull(), "Unable to deserialize lua object - Expected null value"))
                return sol::nullopt;

            return make_object(p_luaState, sol::nil);
        }
        case sol::type::string:
        {
            if (!CHECK(it->value.IsString(), "Unable to deserialize lua string"))
                return sol::nullopt;

            return sol::make_object(p_luaState, std::string(it->value.GetString(), it->value.GetStringLength()));
        }
        case sol::type::number:
        {
            if (!CHECK(it->value.IsNumber(), "Unable to deserialize lua number"))
                return sol::nullopt;

            return sol::make_object(p_luaState, it->value.GetDouble());
        }
        case sol::type::boolean:
        {
            if (!CHECK(it->value.IsBool(), "Unable to deserialize lua bool"))
                return sol::nullopt;

            return sol::make_object(p_luaState, it->value.GetBool());
        }
        case sol::type::table:
        {
            sol::table table(p_luaState, sol::create);

            if (!ComponentRegistry::FromJson(table, it->value, p_scene))
                return sol::nullopt;

            return table;
        }
        case sol::type::userdata:
        case sol::type::lightuserdata:
        {
            if (!CHECK(it->value.IsObject(), "Unable to deserialize lua user type - Json value should be an object"))
                return sol::nullopt;

            const auto& jsonData = it->value;

            auto dataIt = jsonData.FindMember("type");

            if (!CHECK(dataIt != jsonData.MemberEnd() && dataIt->value.IsString(),
                    "Unable to deserialize lua user type - Invalid type string"))
                return sol::nullopt;

            const std::string typeString(dataIt->value.GetString(), dataIt->value.GetStringLength());

            const auto& luaTypes = LuaTypeRegistry::GetInstance();

            if (!CHECK(luaTypes.Contains(typeString), "Unable to deserialize unregistered lua user type %s", typeString.c_str()))
                return sol::nullopt;

            dataIt = jsonData.FindMember("data");

            if (!CHECK(dataIt != jsonData.MemberEnd(), "Unable to deserialize lua user type - Missing data"))
                return sol::nullopt;

            return luaTypes.GetTypeInfo(typeString).FromJson(p_luaState, dataIt->value, p_scene);
        }
        case sol::type::none:
        case sol::type::thread:
        case sol::type::function:
        case sol::type::poly:
        default:
            CHECK(false, "Unable to deserialize lua object - Unsupported type");
            return sol::nullopt;
        }
    }
}

namespace SvCore::ECS
{
    using namespace SvScripting;

    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, LuaScriptList& p_component)
    {
        p_component.m_owner = p_entity;

        LuaContext& context = LuaContext::GetInstance();

        for (const auto& [script, hint] : p_component.m_scripts)
            context.AddScript(script, p_entity, hint);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, LuaScriptList& p_component)
    {
        p_component.Clear();
    }

    template <>
    void ComponentTraits::OnBeforeChange(EntityHandle& p_entity, LuaScriptList& p_component)
    {
        OnRemove(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, LuaScriptList& p_component)
    {
        OnAdd(p_entity, p_component);
    }

    template <>
    bool ComponentRegistry::ToJson(const LuaScriptList& p_value, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        p_writer.StartArray();

        for (const auto& script : p_value.m_scripts | std::views::keys)
        {
            p_writer.StartObject();

            p_writer.Key("name");

            if (!CHECK(p_writer.String(script.c_str(), static_cast<rapidjson::SizeType>(script.size())),
                    "Failed to write lua script name"))
                return false;

            p_writer.Key("data");

            if (!ToJson(p_value.Get(script).m_table, p_writer, p_toSerialized))
                return false;

            if (!CHECK(p_writer.EndObject(), "Failed to serialize lua script"))
                return false;
        }

        return CHECK(p_writer.EndArray(), "Failed to serialize lua script list");
    }

    template <>
    bool ComponentRegistry::FromJson(LuaScriptList& p_out, const JsonValue& p_json, Scene* p_scene)
    {
        p_out.Clear();

        if (!CHECK(p_json.IsArray(), "Unable to deserialize lua script list - Invalid array"))
            return false;

        for (const auto& script : p_json.GetArray())
        {
            auto scriptIt = script.FindMember("name");

            if (!CHECK(scriptIt != script.MemberEnd() && scriptIt->value.IsString(),
                    "Unable to deserialize lua script - Invalid script name"))
                return false;

            std::string name(scriptIt->value.GetString(), scriptIt->value.GetStringLength());
            name = LuaContext::GetModuleName(name);

            if (p_out.m_scripts.contains(name))
            {
                SV_LOG_WARNING("Ignoring duplicate lua script \"%s\"", name.c_str());
                continue;
            }

            scriptIt = script.FindMember("data");

            if (!CHECK(scriptIt != script.MemberEnd(), "Unable to deserialize lua script - Missing script data"))
                return false;

            sol::table table(LuaContext::GetInstance().GetLuaState(), sol::create);

            if (!FromJson(table, scriptIt->value, p_scene))
                return false;

            p_out.m_scripts[name] = table;
        }

        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(const sol::table& p_value, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        if (p_value == sol::nil)
            return p_writer.Null();

        p_writer.StartArray();

        for (const auto& [key, value] : p_value)
        {
            const sol::type valType = value.get_type();

            if (valType == sol::type::function || valType == sol::type::thread)
                continue;

            p_writer.StartObject();

            p_writer.Key("key");
            if (!CHECK(key != sol::nil, "Unable to serialize lua table - Nil key") || !ToJson(key, p_writer, p_toSerialized))
                return false;

            p_writer.Key("value");
            if (!ToJson(value, p_writer, p_toSerialized) || !CHECK(p_writer.EndObject(), "Unable to write lua table member"))
                return false;
        }

        return CHECK(p_writer.EndArray(), "Failed to serialize lua table");
    }

    template <>
    bool ComponentRegistry::FromJson(sol::table& p_out, const JsonValue& p_json, Scene* p_scene)
    {
        if (!CHECK(p_json.IsArray(), "Unable to deserialize lua table - Json value should be an array"))
            return false;

        for (const auto& elem : p_json.GetArray())
        {
            if (!CHECK(elem.IsObject(), "Unable to deserialize lua table member - Json value should be an object"))
                return false;

            auto it = elem.FindMember("key");

            if (!CHECK(it != elem.MemberEnd(), "Unable to deserialize lua table member - Missing key"))
                return false;

            sol::optional key = LuaObjectFromJson(p_out.lua_state(), it->value, p_scene);

            if (!key.has_value() || !CHECK(*key != sol::nil, "Unable to deserialize lua table member - Nil key"))
                return false;

            it = elem.FindMember("value");

            if (!CHECK(it != elem.MemberEnd(), "Unable to deserialize lua table member - Missing value"))
                return false;

            sol::optional val = LuaObjectFromJson(p_out.lua_state(), it->value, p_scene);

            if (!val.has_value())
                return false;

            p_out[*key] = *val;
        }

        return true;
    }

    template <>
    bool ComponentRegistry::ToJson(const sol::object& p_value, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        if (!CHECK(p_value.valid(), "Unable to seriliaze lua object - Invalid value"))
            return false;

        const sol::type objectType = p_value.get_type();

        p_writer.StartObject();
        p_writer.Key("type");
        p_writer.Int(static_cast<int>(objectType));

        p_writer.Key("data");
        bool result;

        switch (objectType)
        {
        case sol::type::lua_nil:
            result = CHECK(p_writer.Null(), "Failed to write lua nil object");
            break;
        case sol::type::string:
        {
            const std::string str = p_value.as<std::string>();
            result                = CHECK(p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size())),
                    "Unable to write lua string");
            break;
        }
        case sol::type::number:
            result = CHECK(p_writer.Double(p_value.as<double>()), "Unable to write lua number");
            break;
        case sol::type::boolean:
            result = CHECK(p_writer.Bool(p_value.as<bool>()), "Unable to write lua bool");
            break;
        case sol::type::table:
            result = ToJson(p_value.as<sol::table>(), p_writer, p_toSerialized);
            break;
        case sol::type::userdata:
        case sol::type::lightuserdata:
        {
            const auto&       userData   = p_value.as<sol::userdata&>();
            const std::string typeString = userData["__type"]["name"];

            const auto& luaTypes = LuaTypeRegistry::GetInstance();

            if (!CHECK(luaTypes.Contains(typeString), "Unable to serialize unregistered lua user type %s", typeString.c_str()))
                return false;

            p_writer.StartObject();

            p_writer.Key("type");
            p_writer.String(typeString.c_str(), static_cast<rapidjson::SizeType>(typeString.size()));

            p_writer.Key("data");

            if (!luaTypes.GetTypeInfo(typeString).ToJson(userData, p_writer, p_toSerialized))
                return false;

            result = CHECK(p_writer.EndObject(), "Failed to write lua user data");
            break;
        }
        case sol::type::none:
        case sol::type::thread:
        case sol::type::function:
        case sol::type::poly:
        default:
            return CHECK(false, "Unable to serialize lua object - Unsupported type");
        }

        return result && CHECK(p_writer.EndObject(), "Failed to write lua object");
    }
}
