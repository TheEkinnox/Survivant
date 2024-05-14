#include "SurvivantPhysics/Collider/BoxCollider.h"

#include "SurvivantPhysics/PhysicsContext.h"

#include <PxShape.h>

using namespace LibMath;
using namespace SvCore::Serialization;
using namespace physx;

namespace SvPhysics
{
    BoundingBox BoxCollider::GetBoundingBox() const
    {
        const Vector3 halfSize = m_size * .5f;

        return BoundingBox
        {
            m_offset - halfSize,
            m_offset + halfSize
        };
    }

    BoundingSphere BoxCollider::GetBoundingSphere() const
    {
        return {
            m_offset,
            max(m_size.m_x, max(m_size.m_y, m_size.m_z))
        };
    }

    bool BoxCollider::ToJsonImpl(JsonWriter& p_writer) const
    {
        p_writer.Key("size");
        return ::ToJson(m_size, p_writer);
    }

    bool BoxCollider::FromJsonImpl(const JsonValue& p_json)
    {
        const auto it = p_json.FindMember("size");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize box collider - Missing size"))
            return false;

        return ::FromJson(m_size, it->value);
    }

    void BoxCollider::Refresh(const Transform* p_transform)
    {
        ICollider::Refresh(p_transform);

        Vector3 halfSize = m_size * .5f;
        Vector3 offset   = m_offset;

        if (p_transform)
        {
            const Vector3 scale = p_transform->getWorldScale();
            halfSize *= scale;
            offset *= scale;
        }

        m_pxShape->setGeometry(PxBoxGeometry(halfSize.m_x, halfSize.m_y, halfSize.m_z));
        m_pxShape->setLocalPose(PxTransform(reinterpret_cast<const PxVec3&>(offset)));
    }
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvPhysics::BoxCollider& p_component)
    {
        SvPhysics::PhysicsContext::GetInstance().AddCollider(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvPhysics::BoxCollider&)
    {
        auto& context = SvPhysics::PhysicsContext::GetInstance();
        context.RemoveCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::BoxCollider>() });
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvPhysics::BoxCollider& p_component)
    {
        auto& context = SvPhysics::PhysicsContext::GetInstance();
        context.UpdateCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::BoxCollider>() }, p_component);
    }
}
