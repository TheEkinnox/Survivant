#pragma once
#include "SurvivantCore/ECS/SceneViewIterator.h"

namespace SvCore::ECS
{
    template <bool IsConst, class... Components>
    SceneViewIterator<IsConst, Components...>::SceneViewIterator(
        iterator_type p_current, iterator_type p_end, const StorageTuple& p_storages)
        : m_storages(&p_storages), m_iterator(p_current), m_end(p_end)
    {
        while (m_iterator != m_end && !IsValid(*m_iterator))
            ++m_iterator;
    }

    template <bool IsConst, class... Components>
    bool SceneViewIterator<IsConst, Components...>::operator==(const SceneViewIterator& p_other) const
    {
        return m_iterator == p_other.m_iterator;
    }

    template <bool IsConst, class... Components>
    bool SceneViewIterator<IsConst, Components...>::operator!=(const SceneViewIterator& p_other) const
    {
        return !(*this == p_other);
    }

    template <bool IsConst, class... Components>
    typename SceneViewIterator<IsConst, Components...>::reference SceneViewIterator<IsConst, Components...>::operator*() const
    {
        return *m_iterator;
    }

    template <bool IsConst, class... Components>
    typename SceneViewIterator<IsConst, Components...>::pointer SceneViewIterator<IsConst, Components...>::operator->() const
    {
        return &operator*();
    }

    template <bool IsConst, class... Components>
    SceneViewIterator<IsConst, Components...>& SceneViewIterator<IsConst, Components...>::operator++()
    {
        while (++m_iterator != m_end && !IsValid(*m_iterator))
        {
            // Do nothing
        }

        return *this;
    }

    template <bool IsConst, class... Components>
    SceneViewIterator<IsConst, Components...> SceneViewIterator<IsConst, Components...>::operator++(int)
    {
        SceneViewIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template <bool IsConst, class... Components>
    template <size_t Index>
    bool SceneViewIterator<IsConst, Components...>::IsValid(const value_type p_entity)
    {
        if (p_entity == NULL_ENTITY)
            return false;

        const auto* storage = std::get<Index>(*m_storages);

        if constexpr (Index == sizeof...(Components) - 1)
            return storage && storage->Contains(p_entity);
        else
            return storage && storage->Contains(p_entity) && IsValid<Index + 1>(p_entity);
    }
}
