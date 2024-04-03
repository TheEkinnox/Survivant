#include "SurvivantCore/ECS/EntityHandle.h"

#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"

namespace SvCore::ECS
{
    EntityHandle::EntityHandle(Scene* p_scene, const Entity p_entity)
        : m_scene(p_scene), m_entity(p_entity)
    {
    }

    EntityHandle::operator bool() const
    {
        return m_scene && m_scene->IsValid(m_entity);
    }

    EntityHandle::operator Entity() const
    {
        return GetEntity();
    }

    Entity EntityHandle::GetEntity() const
    {
        return m_entity;
    }

    Scene* EntityHandle::GetScene() const
    {
        return m_scene;
    }

    EntityHandle EntityHandle::GetRoot() const
    {
        EntityHandle parent = *this;
        EntityHandle root;

        do
        {
            root   = parent;
            parent = parent.GetParent();
        }
        while (parent);

        return root;
    }

    EntityHandle EntityHandle::GetParent() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        return { m_scene, hierarchy ? hierarchy->GetParent() : NULL_ENTITY };
    }

    void EntityHandle::SetParent(const EntityHandle p_parent)
    {
        HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        if (!hierarchy)
        {
            Make<HierarchyComponent>(p_parent);
            return;
        }

        hierarchy->SetParent(p_parent);
        Set<HierarchyComponent>(*hierarchy);
    }

    EntityHandle EntityHandle::GetNextSibling() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        return { m_scene, hierarchy ? hierarchy->GetNextSibling() : NULL_ENTITY };
    }

    EntityHandle EntityHandle::GetPreviousSibling() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        return { m_scene, hierarchy ? hierarchy->GetPreviousSibling() : NULL_ENTITY };
    }

    size_t EntityHandle::GetChildCount() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        return hierarchy ? hierarchy->GetChildCount() : 0;
    }

    EntityHandle EntityHandle::GetChild(size_t p_index) const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        if (!hierarchy || hierarchy->GetChildCount() <= p_index)
            return { m_scene, NULL_ENTITY };

        EntityHandle child = { m_scene, hierarchy->GetFirstChild() };

        while (p_index-- > 0)
            child = child.GetNextSibling();

        return child;
    }

    std::vector<EntityHandle> EntityHandle::GetChildren() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        if (!hierarchy || hierarchy->GetChildCount() == 0)
            return {};

        std::vector<EntityHandle> children;
        EntityHandle              child = { m_scene, hierarchy->GetFirstChild() };

        while (child)
        {
            children.push_back(child);
            child = child.GetNextSibling();
        }

        return children;
    }

    EntityHandle EntityHandle::Copy() const
    {
        if (*this)
            return *this;

        return m_scene->Create(m_entity);
    }

    void EntityHandle::Destroy()
    {
        if (m_scene)
            m_scene->Destroy(m_entity);

        m_entity = NULL_ENTITY;
    }
}
