#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#include <sol/sol.hpp>

namespace SvScripting
{
    struct LuaTypeInfo
    {
        using EntitiesMap = SvCore::ECS::ComponentRegistry::EntitiesMap;
        using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

        /* Lua conversion */
        sol::userdata (*ToLua)(void* p_object, sol::state& p_luaState);

        bool (*FromLua)(void* p_out, const sol::userdata& p_object);

        /* Json serialization */
        bool (*ToJson)(const sol::userdata& p_component, JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

        sol::optional<sol::object> (*FromJson)(lua_State* p_luaState, const rapidjson::Value& p_json, SvCore::ECS::Scene* p_scene);
    };

    class LuaTypeRegistry final : public SvCore::Utility::TypeRegistry<LuaTypeInfo>
    {
    public:
        static LuaTypeRegistry& GetInstance();

        template <typename T>
        LuaTypeInfo& RegisterType(const std::string& p_name);
    };
}

#include "SurvivantScripting/LuaTypeRegistry.inl"
