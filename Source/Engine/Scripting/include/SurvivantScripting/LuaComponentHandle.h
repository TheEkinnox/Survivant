#pragma once
#include <SurvivantCore/ECS/EntityHandle.h>

namespace SvScripting
{
    struct LuaComponentHandle
    {
        SvCore::ECS::EntityHandle m_owner;
        SvCore::Utility::TypeId   m_typeId;

        /**
         * \brief Checks whether the given handle references the same component as this one or not
         * \param p_other The component handle to compare against
         * \return True if the given handle references the same component. False otherwise
         */
        bool operator==(const LuaComponentHandle& p_other) const;

        /**
         * \brief Checks whether the component handle is valid or not
         */
        operator bool() const;
    };
}

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const SvScripting::LuaComponentHandle& p_value, Serialization::JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

    template <>
    bool ComponentRegistry::FromJson(SvScripting::LuaComponentHandle& p_out, const Serialization::JsonValue& p_json, Scene* p_scene);
}
