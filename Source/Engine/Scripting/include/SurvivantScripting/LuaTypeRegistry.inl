#pragma once
#include "SurvivantCore/Resources/ResourceRef.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

namespace SvScripting
{
    inline LuaTypeRegistry& LuaTypeRegistry::GetInstance()
    {
        static LuaTypeRegistry instance;
        return instance;
    }

    template <typename T>
    LuaTypeInfo& LuaTypeRegistry::RegisterType(const std::string& p_name)
    {
        using namespace SvCore::ECS;
        using namespace SvCore::Resources;

        using EntitiesMap = LuaTypeInfo::EntitiesMap;
        using JsonWriter = LuaTypeInfo::JsonWriter;

        const LuaTypeInfo typeInfo
        {
            .ToLua = [](void* p_object, sol::state& p_luaState)
            -> sol::userdata
            {
                if (!p_object)
                    return make_object_userdata(p_luaState, sol::nil);

                return sol::make_object_userdata(p_luaState, static_cast<T*>(p_object));
            },
            .FromLua = [](void* p_out, const sol::userdata& p_object)
            {
                if (!p_out || !p_object.is<T>())
                    return false;

                *static_cast<T*>(p_out) = *p_object.as<T*>();

                return true;
            },
            .ToJson = [](const sol::userdata& p_object, JsonWriter& p_writer, [[maybe_unused]] const EntitiesMap& p_toSerialized)
            {
                if (!p_object.valid() || !p_object.is<T>())
                    return false;

                const T& obj = *p_object.as<T*>();

                return ComponentRegistry::ToJson<T>(obj, p_writer, p_toSerialized);
            },
            .FromJson =
            [](lua_State* p_luaState, const SvCore::Serialization::JsonValue& p_json, Scene* p_scene)
            -> sol::optional<sol::object>
            {
                T out;

                if (ComponentRegistry::FromJson(out, p_json, p_scene))
                    return sol::make_object(p_luaState, out);

                return sol::nullopt;
            }
        };

        return TypeRegistry::RegisterType<T>(p_name, typeInfo);
    }
}
