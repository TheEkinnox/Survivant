#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/Serialization/Serializer.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#include <sol/sol.hpp>

namespace SvScripting
{
    struct LuaTypeInfo
    {
        using EntitiesMap = SvCore::ECS::ComponentRegistry::EntitiesMap;
        using JsonWriter = SvCore::Serialization::JsonWriter;
        using JsonValue = SvCore::Serialization::JsonValue;

        /* Lua conversion */
        sol::userdata (*ToLua)(void* p_object, sol::state& p_luaState);

        bool (*FromLua)(void* p_out, const sol::userdata& p_object);

        /* Json serialization */
        bool (*ToJson)(const sol::userdata& p_component, JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

        sol::optional<sol::object> (*FromJson)(lua_State* p_luaState, const JsonValue& p_json, SvCore::ECS::Scene* p_scene);
    };

    class LuaTypeRegistry final : public SvCore::Utility::TypeRegistry<LuaTypeInfo>
    {
    public:
        /**
         * \brief Gets the current lua type registry
         * \return A reference to the current lua type registry
         */
        static LuaTypeRegistry& GetInstance();

        /**
         * \brief Registers the given type to the lua type registry with the given name
         * \tparam T The registered type
         * \param p_name The name under which the type should be registered
         * \return The registered type's information
         */
        template <typename T>
        LuaTypeInfo& RegisterType(const std::string& p_name);
    };
}

#include "SurvivantScripting/LuaTypeRegistry.inl"
