#pragma once
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#include <ranges>
#include <type_traits>

namespace SvCore::Utility
{
    template <class TypeInfo>
    template <typename T>
    TypeId TypeRegistry<TypeInfo>::GetTypeId()
    {
        return typeid(std::remove_cvref_t<T>).hash_code();
    }

    template <class TypeInfo>
    template <typename T>
    TypeInfo& TypeRegistry<TypeInfo>::RegisterType(const std::string& p_name, const TypeInfo& p_info)
    {
        ASSERT(!m_typeIds.contains(p_name), "Type name \"%s\" has already been registered", p_name.c_str());

        TypeId id = GetTypeId<T>();
        ASSERT(!m_typeInfos.contains(id), "Type %llu (\"%s\") has already been registered", id, typeid(T).name());

        m_typeNames[id]   = p_name;
        m_typeIds[p_name] = id;
        return (m_typeInfos[id] = p_info);
    }

    template <class TypeInfo>
    std::vector<std::string> TypeRegistry<TypeInfo>::GetRegisteredNames() const
    {
        const auto& view = m_typeNames | std::ranges::views::values;
        return { view.begin(), view.end() };
    }

    template <class TypeInfo>
    bool TypeRegistry<TypeInfo>::Contains(const std::string& p_name) const
    {
        return m_typeIds.contains(p_name);
    }

    template <class TypeInfo>
    bool TypeRegistry<TypeInfo>::Contains(const TypeId& p_id) const
    {
        return m_typeInfos.contains(p_id);
    }

    template <class TypeInfo>
    template <typename T>
    bool TypeRegistry<TypeInfo>::Contains() const
    {
        return Contains(GetTypeId<T>());
    }

    template <class TypeInfoT>
    TypeInfoT& TypeRegistry<TypeInfoT>::GetTypeInfo(const std::string& p_type)
    {
        const auto it = m_typeIds.find(p_type);
        ASSERT(it != m_typeIds.end(), "No registered type \"%s\" found.", p_type.c_str());
        return GetTypeInfo(it->second);
    }

    template <typename TypeInfo>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo(const std::string& p_type) const
    {
        return const_cast<TypeRegistry*>(this)->GetTypeInfo(p_type);
    }

    template <class TypeInfoT>
    TypeInfoT& TypeRegistry<TypeInfoT>::GetTypeInfo(const size_t p_typeId)
    {
        const auto it = m_typeInfos.find(p_typeId);
        ASSERT(it != m_typeInfos.end(), "No registered type id \"%llu\" found.", p_typeId);
        return it->second;
    }

    template <typename TypeInfo>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo(const size_t p_typeId) const
    {
        return const_cast<TypeRegistry*>(this)->GetTypeInfo(p_typeId);
    }

    template <class TypeInfoT>
    template <typename T>
    TypeInfoT& TypeRegistry<TypeInfoT>::GetTypeInfo()
    {
        return GetTypeInfo(GetTypeId<T>());
    }

    template <typename TypeInfo>
    template <typename T>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo() const
    {
        return GetTypeInfo(GetTypeId<T>());
    }

    template <typename TypeInfo>
    const std::string& TypeRegistry<TypeInfo>::GetRegisteredTypeName(const size_t p_typeId) const
    {
        const auto it = m_typeNames.find(p_typeId);
        ASSERT(it != m_typeNames.end(), "Couldn't find registered name of type with id %llu", p_typeId);

        return it->second;
    }

    template <typename TypeInfo>
    template <typename T>
    const std::string& TypeRegistry<TypeInfo>::GetRegisteredTypeName() const
    {
        return GetRegisteredTypeName(GetTypeId<T>());
    }
}
