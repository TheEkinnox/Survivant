#include "SurvivantScripting/LuaScriptComponent.h"

#include "SurvivantScripting/LuaContext.h"

namespace SvCore::ECS
{
    using namespace SvScripting;

    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, LuaScriptComponent& p_component)
    {
        LuaContext& context = LuaContext::GetInstance();
        (void)context.AddScript(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, LuaScriptComponent&)
    {
        LuaContext& context = LuaContext::GetInstance();
        context.RemoveScript(p_entity);
    }

    template <>
    void ComponentTraits::OnBeforeChange(EntityHandle& p_entity, LuaScriptComponent& p_component)
    {
        OnRemove(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, LuaScriptComponent& p_component)
    {
        OnAdd(p_entity, p_component);
    }
}
