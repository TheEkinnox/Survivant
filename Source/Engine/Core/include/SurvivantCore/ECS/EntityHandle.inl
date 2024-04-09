#pragma once
#include "SurvivantCore/ECS/EntityHandle.h"

#include "SurvivantCore/ECS/Scene.h"

namespace SvCore::ECS
{
    template <typename T>
    bool EntityHandle::Has() const
    {
        return m_scene ? m_scene->Has<T>(m_entity) : false;
    }

    template <typename T>
    T* EntityHandle::Get()
    {
        return m_scene ? m_scene->Get<T>(m_entity) : nullptr;
    }

    template <typename T>
    const T* EntityHandle::Get() const
    {
        return m_scene ? static_cast<const Scene*>(m_scene)->Get<T>(m_entity) : nullptr;
    }

    template <typename T>
    T* EntityHandle::GetInParent()
    {
        if (T* component = Get<T>())
            return component;

        EntityHandle parent = GetParent();

        while (parent)
        {
            if (T* component = parent.GetInParent<T>())
                return component;

            parent = parent.GetParent();
        }

        return nullptr;
    }

    template <typename T>
    const T* EntityHandle::GetInParent() const
    {
        if (const T* component = Get<T>())
            return component;

        EntityHandle parent = GetParent();

        while (parent)
        {
            if (const T* component = parent.GetInParent<T>())
                return component;

            parent = parent.GetParent();
        }

        return nullptr;
    }

    template <typename T>
    T* EntityHandle::GetInChildren()
    {
        if (T* component = Get<T>())
            return component;

        const std::vector<EntityHandle> children = GetChildren();

        for (const EntityHandle& child : children)
        {
            if (T* component = child.GetInChildren<T>())
                return component;
        }

        return nullptr;
    }

    template <typename T>
    const T* EntityHandle::GetInChildren() const
    {
        if (const T* component = Get<T>())
            return component;

        const std::vector<EntityHandle> children = GetChildren();

        for (const EntityHandle& child : children)
        {
            if (const T* component = child.GetInChildren<T>())
                return component;
        }

        return nullptr;
    }

    template <typename T>
    T* EntityHandle::GetInHierarchy(const EComponentSearchOrigin p_searchOrigin)
    {
        switch (p_searchOrigin)
        {
        case EComponentSearchOrigin::ROOT:
        {
            return GetRoot().GetInChildren<T>();
        }
        case EComponentSearchOrigin::PARENT:
        {
            if (T* component = GetInParent<T>())
                return component;

            return GetInChildren<T>();
        }
        case EComponentSearchOrigin::CHILDREN:
        {
            if (T* component = GetInChildren<T>())
                return component;

            return GetInParent<T>();
        }
        default:
            ASSERT(false, "Invalid component search origin");
            return nullptr;
        }
    }

    template <typename T>
    const T* EntityHandle::GetInHierarchy(const EComponentSearchOrigin p_searchOrigin) const
    {
        switch (p_searchOrigin)
        {
        case EComponentSearchOrigin::ROOT:
        {
            return GetRoot().GetInChildren<T>();
        }
        case EComponentSearchOrigin::PARENT:
        {
            if (const T* component = GetInParent<T>())
                return component;

            return GetInChildren<T>();
        }
        case EComponentSearchOrigin::CHILDREN:
        {
            if (const T* component = GetInChildren<T>())
                return component;

            return GetInParent<T>();
        }
        default:
            ASSERT(false, "Invalid component search origin");
            return nullptr;
        }
    }

    template <typename T>
    T& EntityHandle::Set(const T& p_instance)
    {
        ASSERT(*this);
        return m_scene->Set<T>(m_entity, p_instance);
    }

    template <typename T, typename... Args>
    T& EntityHandle::Make(Args&&... p_args)
    {
        ASSERT(*this);
        return m_scene->Make<T>(m_entity, std::forward<Args>(p_args)...);
    }

    template <typename T>
    void EntityHandle::Remove()
    {
        if (m_scene)
            m_scene->Remove<T>(m_entity);
    }

    template <typename T>
    void EntityHandle::Remove(const T& p_instance)
    {
        if (m_scene)
            m_scene->Remove<T>(p_instance);
    }
}