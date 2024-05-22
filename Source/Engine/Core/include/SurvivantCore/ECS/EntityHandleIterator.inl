#pragma once
#include "SurvivantCore/ECS/EntityHandleIterator.h"

namespace SvCore::ECS
{
    template <bool IsReverse>
    EntityHandleIterator<IsReverse>::EntityHandleIterator(const EntityHandle p_entity)
        : m_entity(p_entity)
    {
    }

    template <bool IsReverse>
    bool EntityHandleIterator<IsReverse>::operator==(const EntityHandleIterator& p_other) const
    {
        return p_other.m_entity == m_entity;
    }

    template <bool IsReverse>
    typename EntityHandleIterator<IsReverse>::reference EntityHandleIterator<IsReverse>::operator*() const
    {
        return const_cast<reference>(m_entity);
    }

    template <bool IsReverse>
    typename EntityHandleIterator<IsReverse>::pointer EntityHandleIterator<IsReverse>::operator->() const
    {
        return const_cast<pointer>(&m_entity);
    }

    template <bool IsReverse>
    EntityHandleIterator<IsReverse>& EntityHandleIterator<IsReverse>::operator++()
    {
        if constexpr (IsReverse)
            m_entity = m_entity.GetPreviousSibling();
        else
            m_entity = m_entity.GetNextSibling();

        return *this;
    }

    template <bool IsReverse>
    EntityHandleIterator<IsReverse> EntityHandleIterator<IsReverse>::operator++(int)
    {
        EntityHandle tmp = m_entity;
        return (++(*this), tmp);
    }

    template <bool IsReverse>
    EntityHandleIterator<IsReverse>& EntityHandleIterator<IsReverse>::operator--()
    {
        if constexpr (IsReverse)
            m_entity = m_entity.GetNextSibling();
        else
            m_entity = m_entity.GetPreviousSibling();

        return *this;
    }

    template <bool IsReverse>
    EntityHandleIterator<IsReverse> EntityHandleIterator<IsReverse>::operator--(int)
    {
        EntityHandle tmp = m_entity;
        return (--(*this), tmp);
    }
}
