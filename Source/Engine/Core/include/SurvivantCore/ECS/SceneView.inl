#pragma once
#include "SurvivantCore/ECS/SceneView.h"

#include "SurvivantCore/ECS/Scene.h"

namespace SvCore::ECS
{
    template <class... Components>
    SceneView<Components...>::SceneView(SceneRef p_scene)
        : m_scene(&p_scene)
    {
        InitializeView<Components...>();
    }

    template <class... Components>
    void SceneView<Components...>::Refresh()
    {
        InitializeView<Components...>();
    }

    template <class... Components>
    template <typename... T>
    std::enable_if_t<(sizeof...(T) > 1), std::tuple<T*...>> SceneView<Components...>::Get(const Entity p_owner)
    {
        return std::make_tuple(Get<T>(p_owner)...);
    }

    template <class... Components>
    template <typename T>
    T* SceneView<Components...>::Get(const Entity p_owner)
    {
        static_assert(Has<T>);
        return GetStorage<T>().Find(p_owner);
    }

    template <class... Components>
    std::tuple<Components*...> SceneView<Components...>::Get(const Entity p_owner)
    {
        return Get<Components...>(p_owner);
    }

    template <class... Components>
    template <typename T>
    ComponentStorage<T>& SceneView<Components...>::GetStorage()
    {
        static_assert(Has<T>);
        if constexpr (Utility::IsOneOf<T, Components...>)
            return *std::get<IndexOf<T>>(m_storages);
        else
            return *reinterpret_cast<ComponentStorage<T>*>(std::get<IndexOf<std::remove_const_t<T>>>(m_storages));
    }

    template <class... Components>
    template <typename T>
    void SceneView<Components...>::SetStorage(ComponentStorage<T>& p_storage)
    {
        static_assert(Has<T>);
        if constexpr (Utility::IsOneOf<T, Components...>)
            std::get<IndexOf<T>>(m_storages) = &p_storage;
        else
            std::get<IndexOf<std::remove_const_t<T>>>(m_storages) = &p_storage;
    }

    template <class... Components>
    typename SceneView<Components...>::iterator SceneView<Components...>::begin()
    {
        EntityStorageRef entities = m_scene->template GetStorage<Entity>();
        return iterator(entities.begin(), entities.end(), m_storages);
    }

    template <class... Components>
    typename SceneView<Components...>::iterator SceneView<Components...>::end()
    {
        EntityStorageRef entities = m_scene->template GetStorage<Entity>();
        return iterator(entities.end(), entities.end(), m_storages);
    }

    template <class... Components>
    typename SceneView<Components...>::const_iterator SceneView<Components...>::begin() const
    {
        const EntityStorage& entities = m_scene->template GetStorage<Entity>();
        return const_iterator(entities.begin(), entities.end(), m_storages);
    }

    template <class... Components>
    typename SceneView<Components...>::const_iterator SceneView<Components...>::end() const
    {
        const EntityStorage& entities = m_scene->template GetStorage<Entity>();
        return const_iterator(entities.end(), entities.end(), m_storages);
    }

    template <class... Components>
    template <typename T, typename... Remainder>
    void SceneView<Components...>::InitializeView()
    {
        SetStorage<T>(const_cast<ComponentStorage<T>&>(m_scene->template GetStorage<T>()));

        if constexpr (sizeof...(Remainder) > 0)
            InitializeView<Remainder...>();
    }
}
