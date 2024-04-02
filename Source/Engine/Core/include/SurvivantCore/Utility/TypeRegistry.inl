#pragma once
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

namespace SvCore::Utility
{
    template <class TypeInfo>
    template <typename T>
    void TypeRegistry<TypeInfo>::RegisterType(const std::string& p_name, const TypeInfo& p_info)
    {
        ASSERT(!m_typeIds.contains(p_name), "Type name \"%s\" has already been registered", p_name.c_str());

        TypeId id = typeid(T).hash_code();
        ASSERT(!m_typeInfos.contains(id), "Type %llu (\"%s\") has already been registered", id, typeid(T).name());

        m_typeInfos[id]   = p_info;
        m_typeNames[id]   = p_name;
        m_typeIds[p_name] = id;
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
        return Contains(typeid(T).hash_code());
    }

    template <typename TypeInfo>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo(const std::string& p_type) const
    {
        const auto it = m_typeIds.find(p_type);
        ASSERT(it != m_typeIds.end(), "No registered type \"%s\" found.", p_type.c_str());
        return GetTypeInfo(it->second);
    }

    template <typename TypeInfo>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo(const size_t p_typeId) const
    {
        const auto it = m_typeInfos.find(p_typeId);
        ASSERT(it != m_typeInfos.end(), "No registered type id \"%llu\" found.", p_typeId);
        return it->second;
    }

    template <typename TypeInfo>
    template <typename T>
    const TypeInfo& TypeRegistry<TypeInfo>::GetTypeInfo() const
    {
        return GetTypeInfo(typeid(T).hash_code());
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
        return GetRegisteredTypeName(typeid(T).hash_code());
    }
}
