#include "SurvivantCore/ECS/EntityHandle.h"

#include "SurvivantCore/ECS/ComponentHandle.h"
#include "SurvivantCore/ECS/EntityHandleIterator.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"

using namespace SvCore::Serialization;
using namespace SvCore::Utility;

namespace SvCore::ECS
{
    EntityHandle::EntityHandle(Scene* p_scene, const Entity p_entity)
        : m_scene(p_scene), m_entity(p_entity)
    {
    }

    bool EntityHandle::operator==(const EntityHandle& p_other) const
    {
        return m_entity == p_other.m_entity && m_scene == p_other.m_scene;
    }

    bool EntityHandle::operator==(const Entity p_entity) const
    {
        return m_entity == p_entity;
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

    std::string EntityHandle::GetDisplayName() const
    {
        if (!*this)
            return "None";

        std::ostringstream str;
        if (const TagComponent* tag = Get<TagComponent>(); tag && !tag->m_tag.empty())
            str << tag->m_tag << " (" << m_entity << ")";
        else
            str << "Entity " << m_entity;

        return str.str();
    }

    void EntityHandle::SetParent(const EntityHandle p_parent)
    {
        Make<HierarchyComponent>(p_parent);
    }

    void EntityHandle::SetParent(const EntityHandle p_parent, const bool p_keepWorld)
    {
        LibMath::Transform* transform = Get<LibMath::Transform>();

        if (!transform)
            return SetParent(p_parent);

        if (p_keepWorld)
        {
            const LibMath::Matrix4 world = transform->getWorldMatrix();
            SetParent(p_parent);
            transform->setWorldMatrix(world);
        }
        else
        {
            const LibMath::Matrix4 local = transform->getMatrix();
            SetParent(p_parent);
            transform->setMatrix(local);
        }
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

    Entity::Index EntityHandle::GetChildCount() const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        return hierarchy ? hierarchy->GetChildCount() : 0;
    }

    EntityHandle EntityHandle::GetChild(Entity::Index p_index) const
    {
        const HierarchyComponent* hierarchy = Get<HierarchyComponent>();

        if (!hierarchy || hierarchy->GetChildCount() <= p_index)
            return { m_scene, NULL_ENTITY };

        EntityHandle child = { m_scene, hierarchy->GetFirstChild() };

        while (p_index-- > 0)
            child = child.GetNextSibling();

        return child;
    }

    EntityHandle EntityHandle::AddChild() const
    {
        if (!*this)
            return {};

        EntityHandle child = m_scene->Create();
        child.SetParent(*this);
        return child;
    }

    EntityHandle::iterator EntityHandle::begin() const
    {
        return { GetChild(0) };
    }

    EntityHandle::iterator EntityHandle::end() const
    {
        return { { m_scene, NULL_ENTITY } };
    }

    EntityHandle::reverse_iterator EntityHandle::rbegin() const
    {
        return { GetChild(GetChildCount() - 1) };
    }

    EntityHandle::reverse_iterator EntityHandle::rend() const
    {
        return { { m_scene, NULL_ENTITY } };
    }

    std::vector<EntityHandle> EntityHandle::GetChildren() const
    {
        return { begin(), end() };
    }

    EntityHandle EntityHandle::Copy() const
    {
        if (!*this)
            return {};

        return m_scene->Create(m_entity);
    }

    void EntityHandle::Destroy()
    {
        if (m_scene)
            m_scene->Destroy(m_entity);

        m_entity = NULL_ENTITY;
    }

    bool EntityHandle::Has(const TypeId p_type) const
    {
        if (!m_scene)
            return false;

        return m_scene->GetStorage(p_type).Contains(m_entity);
    }

    ComponentHandle EntityHandle::Get(const TypeId p_type) const
    {
        if (m_scene)
            return { *this, p_type };

        return {};
    }

    ComponentHandle EntityHandle::GetInParent(const TypeId p_type) const
    {
        ComponentHandle current{ *this, p_type };

        if (current)
            return current;

        EntityHandle parent = GetParent();

        while (parent)
        {
            if ((current = { parent, p_type }))
                return current;

            parent = parent.GetParent();
        }

        return {};
    }

    ComponentHandle EntityHandle::GetInChildren(const TypeId p_type) const
    {
        if (const ComponentHandle component = Get(p_type))
            return component;

        for (const EntityHandle& child : *this)
        {
            if (const ComponentHandle component = child.GetInChildren(p_type))
                return component;
        }

        return {};
    }

    ComponentHandle EntityHandle::GetInHierarchy(const TypeId p_type, const EComponentSearchOrigin p_searchOrigin) const
    {
        switch (p_searchOrigin)
        {
        case EComponentSearchOrigin::ROOT:
        {
            return GetRoot().GetInChildren(p_type);
        }
        case EComponentSearchOrigin::PARENT:
        {
            if (const ComponentHandle component = GetInParent(p_type))
                return component;

            return GetInChildren(p_type);
        }
        case EComponentSearchOrigin::CHILDREN:
        {
            if (const ComponentHandle component = GetInChildren(p_type))
                return component;

            return GetInParent(p_type);
        }
        default:
            ASSERT(false, "Invalid component search origin");
            return {};
        }
    }

    ComponentHandle EntityHandle::GetOrCreate(const TypeId p_type) const
    {
        if (!m_scene)
            return {};

        if (m_scene->GetStorage(p_type).GetOrCreateRaw(m_entity))
            return { *this, p_type };

        return {};
    }

    Entity::Id EntityHandle::GetComponentCount() const
    {
        return m_scene ? m_scene->GetComponentCount(m_entity) : 0;
    }

    std::vector<TypeId> EntityHandle::GetComponentIds() const
    {
        return m_scene ? m_scene->GetComponentIds(m_entity) : std::vector<TypeId>();
    }

    std::vector<std::pair<TypeId, void*>> EntityHandle::GetComponents() const
    {
        return m_scene ? m_scene->GetComponents(m_entity) : std::vector<std::pair<TypeId, void*>>();
    }

    std::vector<ComponentHandle> EntityHandle::GetComponentHandles() const
    {
        return m_scene ? m_scene->GetComponentHandles(m_entity) : std::vector<ComponentHandle>{};
    }

    std::ostream& operator<<(std::ostream& p_stream, const EntityHandle& p_handle)
    {
        return p_stream << p_handle.GetEntity();
    }

    template <>
    bool ComponentRegistry::ToJson(const EntityHandle& p_component, JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        Entity entity = p_component.GetEntity();

        if (entity != NULL_ENTITY)
        {
            const auto it = p_toSerialized.find(entity);

            if (!CHECK(it != p_toSerialized.end(), "Unable to serialize entity handle - Entity is not serialized"))
                return false;

            entity = it->second;
        }

        return p_writer.Uint64(entity);
    }

    template <>
    bool ComponentRegistry::FromJson(EntityHandle& p_out, const JsonValue& p_json, Scene* p_scene)
    {
        if (!CHECK(p_json.Is<Entity::Id>(), "Unable to deserialize entity handle - Json value should be castable to Entity::Id"))
            return false;

        p_out = { p_scene, Entity(p_json.Get<Entity::Id>()) };
        return true;
    }
}
