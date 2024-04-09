#pragma once
#include "SurvivantCore/ECS/ComponentStorage.h"

#include "SurvivantCore/ECS/ComponentTraits.h"

namespace SvCore::ECS
{
    template <class T>
    ComponentStorage<T>::ComponentStorage(Scene* p_scene)
        : m_scene(p_scene)
    {
    }

    template <class T>
    bool ComponentStorage<T>::Contains(const Entity p_entity) const
    {
        return m_entityToComponent.contains(p_entity);
    }

    template <class T>
    bool ComponentStorage<T>::Copy(const Entity p_source, const Entity p_target)
    {
        const auto sourceIt = m_entityToComponent.find(p_source);

        if (sourceIt == m_entityToComponent.end())
            return false;

        Set(p_target, m_components[sourceIt->second]);
        return true;
    }

    template <class T>
    T& ComponentStorage<T>::Set(const Entity p_owner, const ComponentT& p_instance)
    {
        const auto   it = m_entityToComponent.find(p_owner);
        EntityHandle handle(m_scene, p_owner);

        if (it != m_entityToComponent.end())
        {
            ComponentT& component = m_components[it->second];

            ComponentTraits::OnBeforeChange<ComponentT>(handle, component);
            m_onBeforeChange.Invoke(handle, component);

            component = p_instance;

            ComponentTraits::OnChange<ComponentT>(handle, component);
            m_onChange.Invoke(handle, component);

            return component;
        }

        ComponentT&  component = m_components.emplace_back(p_instance);
        const size_t index     = m_components.size() - 1;

        m_componentToEntity[index]   = p_owner;
        m_entityToComponent[p_owner] = index;

        ComponentTraits::OnAdd<ComponentT>(handle, component);
        m_onAdd.Invoke(handle, component);

        return component;
    }

    template <class T>
    template <typename... Args>
    T& ComponentStorage<T>::Construct(const Entity p_owner, Args&&... p_args)
    {
        const auto   it = m_entityToComponent.find(p_owner);
        EntityHandle handle(m_scene, p_owner);

        if (it != m_entityToComponent.end())
        {
            ComponentT& component = m_components[it->second];

            ComponentTraits::OnBeforeChange<ComponentT>(handle, component);
            m_onBeforeChange.Invoke(handle, component);

            component = *new(&component) ComponentT(std::forward<Args>(p_args)...);

            ComponentTraits::OnChange<ComponentT>(handle, component);
            m_onChange.Invoke(handle, component);
            return component;
        }

        ComponentT&  component = m_components.emplace_back(std::forward<Args>(p_args)...);
        const size_t index     = m_components.size() - 1;

        m_componentToEntity[index]   = p_owner;
        m_entityToComponent[p_owner] = index;

        ComponentTraits::OnAdd<ComponentT>(handle, component);
        m_onAdd.Invoke(handle, component);

        return component;
    }

    template <class T>
    void ComponentStorage<T>::Remove(const Entity p_owner)
    {
        const auto it = m_entityToComponent.find(p_owner);

        if (it == m_entityToComponent.end())
            return;

        ComponentT&  component = m_components[it->second];
        EntityHandle handle(m_scene, p_owner);

        ComponentTraits::OnRemove<ComponentT>(handle, component);
        m_onRemove.Invoke({ m_scene, p_owner }, component);

        const size_t lastIndex = m_components.size() - 1;

        m_componentToEntity[it->second] = m_componentToEntity[lastIndex];
        std::swap(component, m_components[lastIndex]);
        m_entityToComponent[m_componentToEntity[it->second]] = it->second;

        m_componentToEntity.erase(lastIndex);
        m_components.resize(lastIndex);
        m_entityToComponent.erase(it);
    }

    template <class T>
    void ComponentStorage<T>::Remove(const T& p_component)
    {
        Remove(GetOwner(p_component));
    }

    template <class T>
    void ComponentStorage<T>::Clear()
    {
        m_components.clear();
        m_componentToEntity.clear();
        m_entityToComponent.clear();
    }

    template <class T>
    void ComponentStorage<T>::Reserve(const Entity::Id p_count)
    {
        m_components.reserve(p_count);
    }

    template <class T>
    Entity::Id ComponentStorage<T>::GetCount() const
    {
        return static_cast<Entity::Id>(m_components.size());
    }

    template <class T>
    bool ComponentStorage<T>::Has(const Entity p_owner) const
    {
        return m_entityToComponent.contains(p_owner);
    }

    template <class T>
    T* ComponentStorage<T>::Find(const Entity p_owner)
    {
        const auto it = m_entityToComponent.find(p_owner);
        return it != m_entityToComponent.end() ? &m_components[it->second] : nullptr;
    }

    template <class T>
    const T* ComponentStorage<T>::Find(const Entity p_owner) const
    {
        const auto it = m_entityToComponent.find(p_owner);
        return it != m_entityToComponent.end() ? &m_components[it->second] : nullptr;
    }

    template <class T>
    Entity ComponentStorage<T>::GetOwner(const T& p_component) const
    {
        size_t index = 0;

        while (index < m_components.size())
        {
            if (&m_components[index] == &p_component)
                break;

            ++index;
        }

        const auto it = m_componentToEntity.find(index);
        return it != m_componentToEntity.end() ? it->second : NULL_ENTITY;
    }

    template <class T>
    typename ComponentStorage<T>::iterator ComponentStorage<T>::begin()
    {
        return m_components.begin();
    }

    template <class T>
    typename ComponentStorage<T>::iterator ComponentStorage<T>::end()
    {
        return m_components.end();
    }

    template <class T>
    typename ComponentStorage<T>::const_iterator ComponentStorage<T>::begin() const
    {
        return m_components.begin();
    }

    template <class T>
    typename ComponentStorage<T>::const_iterator ComponentStorage<T>::end() const
    {
        return m_components.end();
    }
}