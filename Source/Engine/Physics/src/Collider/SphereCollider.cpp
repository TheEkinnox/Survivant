#include "SurvivantPhysics/Collider/SphereCollider.h"

#include "SurvivantPhysics/PhysicsContext.h"

#include <PxShape.h>

using namespace LibMath;
using namespace SvCore::Serialization;
using namespace physx;

namespace SvPhysics
{
    BoundingBox SphereCollider::GetBoundingBox() const
    {
        const Vector3 halfSize(m_radius);

        return BoundingBox
        {
            m_offset - halfSize,
            m_offset + halfSize
        };
    }

    BoundingSphere SphereCollider::GetBoundingSphere() const
    {
        return { m_offset, m_radius };
    }

    bool SphereCollider::ToJsonImpl(JsonWriter& p_writer) const
    {
        p_writer.Key("radius");
        return CHECK(p_writer.Double(m_radius), "Failed to serialize sphere collider radius");
    }

    bool SphereCollider::FromJsonImpl(const JsonValue& p_json)
    {
        const auto it = p_json.FindMember("radius");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize sphere collider - Missing radius"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize sphere collider radius - Json value should be a float"))
            return false;

        m_radius = it->value.GetFloat();

        return true;
    }

    void SphereCollider::Refresh(const Transform* p_transform)
    {
        ICollider::Refresh(p_transform);

        float   radius = m_radius;
        Vector3 offset = m_offset;

        if (p_transform)
        {
            const Vector3 scale = p_transform->getWorldScale();
            radius *= max(scale.m_x, max(scale.m_y, scale.m_z));
            offset *= scale;
        }

        m_pxShape->setGeometry(PxSphereGeometry(radius));
        m_pxShape->setLocalPose(PxTransform(reinterpret_cast<const PxVec3&>(offset)));
    }
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvPhysics::SphereCollider& p_component)
    {
        SvPhysics::PhysicsContext::GetInstance().AddCollider(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvPhysics::SphereCollider&)
    {
        auto& context = SvPhysics::PhysicsContext::GetInstance();
        context.RemoveCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::SphereCollider>() });
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvPhysics::SphereCollider& p_component)
    {
        auto& context = SvPhysics::PhysicsContext::GetInstance();
        context.UpdateCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::SphereCollider>() }, p_component);
    }
}
