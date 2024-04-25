#pragma once
#include "SurvivantCore/ECS/Scene.h"

#include "SurvivantCore/ECS/ComponentStorage.h"

namespace SvCore::ECS
{
    template <typename T>
    bool Scene::Has(Entity p_owner) const
    {
        const auto it = m_components.find(ComponentRegistry::GetTypeId<T>());

        if (it == m_components.end())
            return false;

        return reinterpret_cast<ComponentStorage<T>*>(it->second.get())->Has(p_owner);
    }

    template <typename T>
    T* Scene::Get(Entity p_owner)
    {
        const auto it = m_components.find(ComponentRegistry::GetTypeId<T>());

        if (it == m_components.end())
            return nullptr;

        return reinterpret_cast<ComponentStorage<T>*>(it->second.get())->Find(p_owner);
    }

    template <typename T>
    const T* Scene::Get(Entity p_owner) const
    {
        const auto it = m_components.find(ComponentRegistry::GetTypeId<T>());

        if (it == m_components.end())
            return nullptr;

        return reinterpret_cast<const ComponentStorage<T>*>(it->second.get())->Find(p_owner);
    }

    template <typename T>
    T& Scene::Set(Entity p_owner, const T& p_instance)
    {
        return GetStorage<T>().Set(p_owner, p_instance);
    }

    template <typename T, typename... Args>
    T& Scene::Make(Entity p_owner, Args&&... p_args)
    {
        return GetStorage<T>().Construct(p_owner, std::forward<Args>(p_args)...);
    }

    template <typename T>
    void Scene::Remove(Entity p_owner)
    {
        GetStorage<T>().Remove(p_owner);
    }

    template <typename T>
    void Scene::Remove(const T& p_instance)
    {
        GetStorage<T>().Remove(p_instance);
    }

    template <typename T>
    Scene::Storage<T>& Scene::GetStorage()
    {
        if constexpr (std::is_same_v<Entity, T>)
        {
            return m_entities;
        }
        else
        {
            const Utility::TypeId typeId = ComponentRegistry::GetTypeId<T>();
            const auto            it     = m_components.find(typeId);

            if (it != m_components.end())
                return reinterpret_cast<ComponentStorage<T>&>(*it->second);

            auto& storage = *(m_components[typeId] = std::make_shared<ComponentStorage<T>>(this));
            return reinterpret_cast<ComponentStorage<T>&>(storage);
        }
    }

    template <typename T>
    const Scene::Storage<T>& Scene::GetStorage() const
    {
        if constexpr (std::is_same_v<Entity, T>)
        {
            return m_entities;
        }
        else
        {
            const Utility::TypeId typeId = ComponentRegistry::GetTypeId<T>();
            const auto            it     = m_components.find(typeId);

            if (it != m_components.end())
                return reinterpret_cast<const ComponentStorage<T>&>(*it->second);

            const auto& storage = *(m_components[typeId] = std::make_shared<ComponentStorage<T>>(const_cast<Scene*>(this)));
            return reinterpret_cast<const ComponentStorage<T>&>(storage);
        }
    }
}
