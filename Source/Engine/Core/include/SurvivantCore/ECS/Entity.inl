#pragma once
#include "SurvivantCore/ECS/Entity.h"

#include "SurvivantCore/Debug/Assertion.h"

namespace SvCore::ECS
{
    constexpr Entity::Entity(const Id p_id)
        : m_id(p_id)
    {
    }

    constexpr Entity::Entity(const Id p_index, const Version p_version)
        : Entity(Make(p_index, p_version))
    {
    }

    constexpr Entity::operator Id() const
    {
        return m_id;
    }

    constexpr Entity::Version Entity::GetVersion() const
    {
        return m_id >> INDEX_BITS & VERSION_MASK;
    }

    constexpr Entity::Id Entity::GetIndex() const
    {
        return m_id & INDEX_MASK;
    }

    inline void Entity::BumpVersion()
    {
        const Version version = GetVersion();
        ASSERT(version < VERSION_MASK);
        m_id = Make(GetIndex(), version + 1);
    }

    constexpr Entity::Id Entity::Make(const Id p_index, const Id p_version)
    {
        return (p_index & INDEX_MASK) | ((p_version & VERSION_MASK) << INDEX_BITS);
    }
}
