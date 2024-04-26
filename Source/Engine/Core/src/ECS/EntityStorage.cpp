#include "SurvivantCore/ECS/EntityStorage.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/ECS/Scene.h"

namespace SvCore::ECS
{
    EntityStorage::EntityStorage(Scene* p_scene)
        : m_scene(p_scene)
    {
    }

    Entity EntityStorage::Add()
    {
        if (m_entities.size() == m_count)
            return m_entities.emplace_back(m_count++, Entity::Version{});

        Entity& entity = m_entities[m_count++];
        entity.BumpVersion();

        m_onAdd.Invoke({ m_scene, entity });
        return entity;
    }

    void EntityStorage::Remove(const Entity p_entity)
    {
        const auto it = std::ranges::find(*this, p_entity);

        if (it == end())
            return;

        m_onRemove.Invoke({ m_scene, *it });

        std::swap(*it, m_entities[m_count - 1]);
        --m_count;
    }

    bool EntityStorage::Has(const Entity p_entity) const
    {
        return std::ranges::find(*this, p_entity) != end();
    }

    Entity EntityStorage::Find(Entity::Index p_index) const
    {
        const auto it = std::ranges::find_if(*this, [p_index](const Entity p_entity)
        {
            return p_entity.GetIndex() == p_index;
        });

        return it != end() ? *it : NULL_ENTITY;
    }

    void EntityStorage::Clear()
    {
        m_entities.clear();
        m_count = 0;
    }

    void EntityStorage::Reserve(const Entity::Index p_count)
    {
        m_entities.reserve(p_count);
    }

    EntityStorage::iterator EntityStorage::begin()
    {
        return m_entities.begin();
    }

    EntityStorage::iterator EntityStorage::end()
    {
        return m_entities.end() - static_cast<iterator::difference_type>(m_entities.size() - m_count);
    }

    EntityStorage::const_iterator EntityStorage::begin() const
    {
        return m_entities.begin();
    }

    EntityStorage::const_iterator EntityStorage::end() const
    {
        return m_entities.end() - static_cast<const_iterator::difference_type>(m_entities.size() - m_count);
    }

    Entity::Index EntityStorage::size() const
    {
        return m_count;
    }
}
