#pragma once
#include "SurvivantCore/Utility/DynamicTypeInfo.h"

namespace SvCore::Utility
{
    template <class TypeInfo>
    template <typename... Args>
    DynamicTypeInfo<TypeInfo>::DynamicTypeInfo(Args&&... p_args)
        : TypeInfo(std::forward<Args>(p_args)...)
    {
    }

    template <class TypeInfo>
    bool DynamicTypeInfo<TypeInfo>::Add(const std::string& p_name, const Callable& p_callable)
    {
        const auto it = m_callables.find(p_name);

        if (it != m_callables.end())
            return false;

        m_callables[p_name] = p_callable;
        return true;
    }

    template <class TypeInfo>
    void DynamicTypeInfo<TypeInfo>::Replace(const std::string& p_name, const Callable& p_callable)
    {
        m_callables[p_name] = p_callable;
    }

    template <class TypeInfo>
    bool DynamicTypeInfo<TypeInfo>::Remove(const std::string& p_name)
    {
        const auto it = m_callables.find(p_name);

        if (it != m_callables.end())
        {
            m_callables.erase(it);
            return true;
        }

        return false;
    }

    template <class TypeInfo>
    bool DynamicTypeInfo<TypeInfo>::Has(const std::string& p_name) const
    {
        return m_callables.contains(p_name);
    }

    template <class TypeInfo>
    template <typename T>
    DynamicCallResult<T> DynamicTypeInfo<TypeInfo>::Call(const std::string& p_name, void* p_payload) const
    {
        const auto it = m_callables.find(p_name);

        if constexpr (std::is_same_v<void, T>)
            return it != m_callables.end() ? ((void)it->second(p_payload), true) : false;
        else
            return it != m_callables.end() ? std::make_optional(std::any_cast<T>(it->second(p_payload))) : std::nullopt;
    }
}
