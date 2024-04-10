#pragma once
#include "SurvivantScripting/LuaContext.h"

namespace SvScripting
{
    template <typename... Args>
    bool LuaContext::TryCall(LuaScriptComponent& p_component, const std::string& p_name, Args&&... p_args)
    {
        if (!m_isValid || !p_component.m_table.valid())
            return false;

        sol::protected_function func = p_component.m_table[p_name];

        if (!func.valid())
            return false;

        const auto result = func.call(p_component.m_table, std::forward<Args>(p_args)...);

        if (!result.valid())
        {
            [[maybe_unused]] const sol::error err = result;
            return CHECK(false, "Call to function %s of script %s failed - %s",
                    p_name.c_str(), p_component.m_script.GetPath().c_str(), err.what());
        }

        return true;
    }
}
