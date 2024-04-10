#pragma once
#include "SurvivantScripting/LuaScript.h"

#include <SurvivantCore/ECS/ComponentTraits.h>
#include <SurvivantCore/Resources/ResourceRef.h>

#include <sol/sol.hpp>

namespace SvScripting
{
    struct LuaScriptComponent
    {
        SvCore::Resources::ResourceRef<LuaScript> m_script;

        sol::table m_table = sol::nil;
    };
}

namespace SvCore::ECS
{
    /**
     * \brief The action to perform when a lua script component is added
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvScripting::LuaScriptComponent& p_component);

    /**
     * \brief The action to perform when a lua script component is removed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvScripting::LuaScriptComponent& p_component);

    /**
     * \brief The action to perform before a lua script component is changed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnBeforeChange(EntityHandle& p_entity, SvScripting::LuaScriptComponent& p_component);

    /**
     * \brief The action to perform after a lua script component was changed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvScripting::LuaScriptComponent& p_component);
}
