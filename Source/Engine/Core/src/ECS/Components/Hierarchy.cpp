#include "SurvivantCore/ECS/Components/Hierarchy.h"

#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/ECS/SceneView.h"

using namespace LibMath;

namespace SvCore::ECS
{
    HierarchyComponent::HierarchyComponent(const Entity p_parent)
        : m_parent(p_parent)
    {
    }

    Entity HierarchyComponent::GetParent() const
    {
        return m_parent;
    }

    void HierarchyComponent::SetParent(const Entity p_parent)
    {
        m_parent = p_parent;
    }

    Entity HierarchyComponent::GetFirstChild() const
    {
        return m_firstChild;
    }

    Entity HierarchyComponent::GetPreviousSibling() const
    {
        return m_previousSibling;
    }

    Entity HierarchyComponent::GetNextSibling() const
    {
        return m_nextSibling;
    }

    size_t HierarchyComponent::GetChildCount() const
    {
        return m_childCount;
    }

    std::vector<Transform*> GetChildTransforms(const EntityHandle& p_entity)
    {
        std::vector<Transform*> linkedTransforms;

        const std::vector<EntityHandle> children = p_entity.GetChildren();

        for (EntityHandle child : children)
        {
            if (Transform* childTransform = child.Get<Transform>())
            {
                linkedTransforms.push_back(childTransform);
            }
            else
            {
                const std::vector<Transform*> childTransforms = GetChildTransforms(child);
                linkedTransforms.insert(linkedTransforms.end(), childTransforms.begin(), childTransforms.end());
            }
        }

        return linkedTransforms;
    }

    void LinkTransforms(EntityHandle& p_entity)
    {
        Transform* parentTransform = p_entity.GetParent().GetInParent<Transform>();
        Transform* transform       = p_entity.Get<Transform>();

        if (transform)
            transform->setParent(parentTransform, parentTransform == nullptr);

        const std::vector<Transform*> childTransforms = GetChildTransforms(p_entity);

        for (Transform* childTransform : childTransforms)
        {
            Transform* newParent = transform ? transform : parentTransform;
            childTransform->setParent(newParent, parentTransform == nullptr);
        }
    }

    void UnlinkTransforms(const EntityHandle& p_entity)
    {
        Transform* parentTransform = p_entity.GetParent().GetInParent<Transform>();

        const std::vector<Transform*> linkedTransforms = GetChildTransforms(p_entity);

        for (Transform* transform : linkedTransforms)
            transform->setParent(parentTransform, true);
    }

    template <>
    void ComponentTraits::OnAdd<HierarchyComponent>(EntityHandle& p_owner, HierarchyComponent& p_hierarchy)
    {
        ASSERT(p_hierarchy.m_firstChild == NULL_ENTITY, "Adding a pre-existing hierarchy is not supported");
        ASSERT(p_hierarchy.m_previousSibling == NULL_ENTITY, "Adding a pre-existing hierarchy is not supported");
        ASSERT(p_hierarchy.m_nextSibling == NULL_ENTITY, "Adding a pre-existing hierarchy is not supported");
        ASSERT(p_hierarchy.m_childCount == 0, "Adding a pre-existing hierarchy is not supported");

        OnChange(p_owner, p_hierarchy);
    }

    template <>
    void ComponentTraits::OnRemove<HierarchyComponent>(EntityHandle& p_entity, HierarchyComponent& p_hierarchy)
    {
        OnBeforeChange(p_entity, p_hierarchy);

        EntityHandle child(p_entity.GetScene(), p_hierarchy.m_firstChild);

        p_hierarchy.m_parent          = NULL_ENTITY;
        p_hierarchy.m_nextSibling     = NULL_ENTITY;
        p_hierarchy.m_previousSibling = NULL_ENTITY;

        while (child)
        {
            const EntityHandle nextChild = child.GetNextSibling();
            child.Destroy();
            child = nextChild;
        }

        p_hierarchy.m_childCount = 0;
        UnlinkTransforms(p_entity);
    }

    template <>
    void ComponentTraits::OnBeforeChange<HierarchyComponent>(EntityHandle& p_entity, HierarchyComponent& p_hierarchy)
    {
        Scene* scene = p_entity.GetScene();
        ASSERT(scene);

        EntityHandle parent(scene, p_hierarchy.m_parent);
        EntityHandle nextSibling(scene, p_hierarchy.m_nextSibling);
        EntityHandle prevSibling(scene, p_hierarchy.m_previousSibling);

        if (HierarchyComponent* parentHierarchy = parent.Get<HierarchyComponent>())
        {
            --parentHierarchy->m_childCount;

            if (p_entity.GetEntity() == parentHierarchy->m_firstChild)
                parentHierarchy->m_firstChild = nextSibling;
        }

        if (HierarchyComponent* prevHierarchy = prevSibling.Get<HierarchyComponent>())
            prevHierarchy->m_nextSibling = nextSibling;

        if (HierarchyComponent* nextHierarchy = nextSibling.Get<HierarchyComponent>())
            nextHierarchy->m_previousSibling = prevSibling;

        p_hierarchy.m_parent          = NULL_ENTITY;
        p_hierarchy.m_nextSibling     = NULL_ENTITY;
        p_hierarchy.m_previousSibling = NULL_ENTITY;
    }

    template <>
    void ComponentTraits::OnChange<HierarchyComponent>(EntityHandle& p_entity, HierarchyComponent& p_hierarchy)
    {
        Scene* scene = p_entity.GetScene();
        ASSERT(scene);

        EntityHandle parent(scene, p_hierarchy.m_parent);

        if (EntityHandle firstChild(scene, p_hierarchy.m_firstChild); firstChild && !firstChild.Has<HierarchyComponent>())
            firstChild.Make<HierarchyComponent>(p_entity);

        if (!parent)
            return;

        if (HierarchyComponent* parentHierarchy = parent.Get<HierarchyComponent>())
        {
            EntityHandle nextSibling(scene, p_hierarchy.m_nextSibling = parentHierarchy->m_firstChild);

            if (nextSibling)
                nextSibling.Get<HierarchyComponent>()->m_previousSibling = p_entity;

            parentHierarchy->m_firstChild = p_entity;
            ++parentHierarchy->m_childCount;
        }
        else
        {
            parentHierarchy               = &parent.Make<HierarchyComponent>(NULL_ENTITY);
            parentHierarchy->m_firstChild = p_entity;
            parentHierarchy->m_childCount = 1;
        }

        LinkTransforms(p_entity);
    }

    template <>
    void ComponentTraits::OnAdd<Transform>(EntityHandle& p_entity, Transform&)
    {
        LinkTransforms(p_entity);
    }

    template <>
    void ComponentTraits::OnRemove<Transform>(EntityHandle& p_entity, Transform&)
    {
        UnlinkTransforms(p_entity);
    }

    template <>
    void ComponentTraits::OnChange<Transform>(EntityHandle& p_entity, Transform&)
    {
        LinkTransforms(p_entity);
    }
}
