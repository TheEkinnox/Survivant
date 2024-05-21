#pragma once
#include "SurvivantCore/ECS/ComponentStorage.h"

#include "SurvivantCore/ECS/ComponentRegistry.h"
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

        EntityHandle sourceHandle(m_scene, p_source);
        EntityHandle targetHandle(m_scene, p_target);

        const Entity::Index index = sourceIt->second;

        ComponentT copyResult = ComponentTraits::Copy<ComponentT>(sourceHandle, m_components[index], targetHandle);
        m_onCopy.Invoke(sourceHandle, m_components[index], targetHandle, copyResult);

        Construct(p_target, std::move(copyResult));
        return true;
    }

    template <class T>
    T& ComponentStorage<T>::Set(Entity p_owner, ComponentT p_instance)
    {
        return Construct(p_owner, std::move(p_instance));
    }

    template <class T>
    template <typename... Args>
    T& ComponentStorage<T>::Construct(const Entity p_owner, Args&&... p_args)
    {
        const auto   it = m_entityToComponent.find(p_owner);
        EntityHandle handle(m_scene, p_owner);

        // Re-fetch component between events in case one of them triggers a reallocation
        if (it != m_entityToComponent.end())
        {
            const Entity::Index index = it->second;
            ComponentT          newVal(std::forward<Args>(p_args)...);

            ComponentTraits::OnBeforeChange<ComponentT>(handle, m_components[index], newVal);
            m_onBeforeChange.Invoke(handle, m_components[index], newVal);

            ComponentTraits::OnChange<ComponentT>(handle, *new(m_components.data() + index) ComponentT(std::move(newVal)));
            m_onChange.Invoke(handle, m_components[index]);
            return m_components[index];
        }

        const Entity::Index index = size();

        m_componentToEntity[index]   = p_owner;
        m_entityToComponent[p_owner] = index;

        ComponentTraits::OnAdd<ComponentT>(handle, m_components.emplace_back(std::forward<Args>(p_args)...));
        m_onAdd.Invoke(handle, m_components[index]);

        return m_components[index];
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
        m_onRemove.Invoke(handle, component);

        const Entity::Index lastIndex = size() - 1;

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
        for (Entity::Index i = size(); i > 0; --i)
        {
            if (i > size())
                continue;

            EntityHandle handle(m_scene, m_componentToEntity[i - 1]);

            if (!handle)
                continue;

            ComponentT& component = m_components[i - 1];

            ComponentTraits::OnRemove<ComponentT>(handle, component);
            m_onRemove.Invoke(handle, component);
        }

        m_components.clear();
        m_componentToEntity.clear();
        m_entityToComponent.clear();
    }

    template <class T>
    void ComponentStorage<T>::Reserve(const Entity::Index p_count)
    {
        m_components.reserve(p_count);
    }

    template <class T>
    void ComponentStorage<T>::Sort()
    {
        std::vector<ComponentT> components;

        components.reserve(size());

        Entity::Index index = 0;
        for (const Entity& entity : m_scene->GetStorage<Entity>())
        {
            const auto it = m_entityToComponent.find(entity);

            if (it == m_entityToComponent.end())
                continue;

            components.emplace_back(std::move(m_components[it->second]));

            m_componentToEntity[index] = entity;
            it->second                 = index;
            ++index;
        }

        m_components = std::move(components);
    }

    template <class T>
    Entity::Index ComponentStorage<T>::size() const
    {
        return static_cast<Entity::Index>(m_components.size());
    }

    template <class T>
    bool ComponentStorage<T>::Has(const Entity p_owner) const
    {
        return m_entityToComponent.contains(p_owner);
    }

    template <class T>
    void* ComponentStorage<T>::GetOrCreateRaw(const Entity p_owner)
    {
        void* out = FindRaw(p_owner);
        return out ? out : (void*)&Construct(p_owner);
    }

    template <class T>
    void* ComponentStorage<T>::FindRaw(const Entity p_owner)
    {
        return (void*)Find(p_owner);
    }

    template <class T>
    const void* ComponentStorage<T>::FindRaw(const Entity p_owner) const
    {
        return (const void*)Find(p_owner);
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
        Entity::Index index = 0;

        while (index < size())
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

    template <class T>
    bool ComponentStorage<T>::ToJson(Serialization::JsonWriter& p_writer, const EntitiesMap& p_entitiesMap) const
    {
        p_writer.StartArray();

        for (const auto [index, entity] : m_componentToEntity)
        {
            const auto it = p_entitiesMap.find(entity);

            if (!CHECK(it != p_entitiesMap.end(), "Failed to serialize component storage - Entity %d not found", entity.GetIndex()))
                return false;

            p_writer.StartObject();

            p_writer.Key("owner");
            if (!CHECK(p_writer.Uint64(it->second), "Failed to write component owner"))
                return false;

            p_writer.Key("data");
            if (!ComponentRegistry::ToJson(m_components[index], p_writer, p_entitiesMap))
                return false;

            p_writer.EndObject();
        }

        return p_writer.EndArray();
    }

    template <class T>
    bool ComponentStorage<T>::FromJson(const Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsArray(), "Failed to deserialize component storage - Json value should be an array"))
            return false;

        for (const auto& jsonComponent : p_json.GetArray())
        {
            if (!CHECK(jsonComponent.IsObject(), "Failed to deserialize storage component - Json value should be an object"))
                return false;

            auto it = jsonComponent.FindMember("owner");

            if (!CHECK(it != jsonComponent.MemberEnd() && it->value.Is<Entity::Id>(), "Failed to read component owner"))
                return false;

            Entity owner(it->value.Get<Entity::Id>());

            it = jsonComponent.FindMember("data");

            if (!CHECK(it != jsonComponent.MemberEnd(), "Failed to read component"))
                return false;

            ComponentT component;
            if (!ComponentRegistry::FromJson(component, it->value, m_scene))
                return false;

            Construct(owner, std::move(component));
        }

        return true;
    }
}
