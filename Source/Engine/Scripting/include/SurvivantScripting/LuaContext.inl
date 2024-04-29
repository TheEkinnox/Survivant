#pragma once
#include "SurvivantScripting/LuaContext.h"

namespace SvScripting
{
    template <typename... Args>
    ELuaCallResult LuaContext::TryCall(sol::table& p_table, const std::string& p_name, Args&&... p_args)
    {
        if (!m_isValid || !p_table.valid())
            return ELuaCallResult::INVALID_STATE;

        sol::protected_function func = p_table[p_name];

        if (!func.valid())
            return ELuaCallResult::NOT_FOUND;

        const auto result = func.call(p_table, std::forward<Args>(p_args)...);

        if (!result.valid())
        {
            [[maybe_unused]] const sol::error err = result;
            CHECK(false, "Call to lua script function %s failed - %s", p_name.c_str(), err.what());
            return ELuaCallResult::FAILURE;
        }

        return ELuaCallResult::SUCCESS;
    }
}
