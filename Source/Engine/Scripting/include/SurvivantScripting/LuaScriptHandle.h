#pragma once
#include "SurvivantScripting/LuaScript.h"

#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/Resources/ResourceRef.h>

#include <sol/table.hpp>

namespace SvScripting
{
    struct LuaScriptHandle
    {
        std::string                               m_name;
        SvCore::Resources::ResourceRef<LuaScript> m_script;
        SvCore::ECS::EntityHandle                 m_owner;
        sol::table                                m_table = sol::nil;

        /**
         * \brief Checks whether this script handle should be ordered before the given one or not
         * \param p_other The script handle to compare against
         * \return True if this script handle should be ordered before the given one. False otherwise
         */
        bool operator<(const LuaScriptHandle& p_other) const;

        /**
         * \brief Checks whether the given script handle references the same script as this one or not
         * \param p_other The script handle to compare against
         * \return True if the given script handle references the same script as this one. False otherwise
         */
        bool operator==(const LuaScriptHandle& p_other) const;

        /**
         * \brief Checks whether the script handle is valid or not
         */
        operator bool() const;
    };
}

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const SvScripting::LuaScriptHandle& p_value, Serialization::JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

    template <>
    bool ComponentRegistry::FromJson(SvScripting::LuaScriptHandle& p_out, const Serialization::JsonValue& p_json, Scene* p_scene);
}
