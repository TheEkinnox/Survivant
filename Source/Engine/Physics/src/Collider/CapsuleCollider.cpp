#include "SurvivantPhysics/Collider/CapsuleCollider.h"

#include "SurvivantPhysics/PhysicsContext.h"

#include <PxShape.h>

#include <foundation/PxTransform.h>

using namespace LibMath;
using namespace SvCore::Serialization;
using namespace physx;

namespace SvPhysics
{
    BoundingBox CapsuleCollider::GetBoundingBox() const
    {
        const Matrix4 rotationMat = rotationFromTo(Vector3::up(), m_axis);
        const Vector3 rightDir    = (rotationMat * Vector4::right()).xyz();
        const Vector3 frontDir    = (rotationMat * Vector4::front()).xyz();

        const Vector3 halfSize = m_axis * (.5f * m_height + m_radius) + rightDir * m_radius + frontDir * m_radius;

        return
        {
            m_offset - halfSize,
            m_offset + halfSize
        };
    }

    LibMath::BoundingBox CapsuleCollider::GetUpwardsBoundingBox() const
    {
        const Vector3 upDir = Vector3::up();
        const Vector3 rightDir = Vector3::right();
        const Vector3 frontDir = Vector3::front();

        const Vector3 halfSize = upDir * (.5f * m_height + m_radius) + rightDir * m_radius + frontDir * m_radius;

        return
        {
            m_offset - halfSize,
            m_offset + halfSize
        };
    }

    BoundingSphere CapsuleCollider::GetBoundingSphere() const
    {
        return
        {
            m_offset,
            m_height * .5f
        };
    }

    bool CapsuleCollider::ToJsonImpl(JsonWriter& p_writer) const
    {
        p_writer.Key("axis");
        if (!::ToJson(m_axis, p_writer))
            return false;

        p_writer.Key("height");
        if (!CHECK(p_writer.Double(m_height), "Failed to write capsule collider height"))
            return false;

        p_writer.Key("radius");
        return CHECK(p_writer.Double(m_radius), "Failed to write capsule collider radius");
    }

    bool CapsuleCollider::FromJsonImpl(const JsonValue& p_json)
    {
        auto it = p_json.FindMember("axis");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize capsule collider - Missing axis"))
            return false;

        if (!::FromJson(m_axis, it->value))
            return false;

        it = p_json.FindMember("height");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize capsule collider - Missing height"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize capsule collider height - Json value should be a float"))
            return false;

        m_height = it->value.GetFloat();

        it = p_json.FindMember("radius");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize capsule collider - Missing radius"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize capsule collider radius - Json value should be a float"))
            return false;

        m_radius = it->value.GetFloat();

        return true;
    }

    void CapsuleCollider::Refresh(const Transform* p_transform)
    {
        ICollider::Refresh(p_transform);

        float   radius     = m_radius;
        float   halfHeight = m_height * .5f;
        Vector3 offset     = m_offset;

        const Quaternion rotation = Quaternion::fromTo(Vector3::right(), m_axis);

        if (p_transform)
        {
            Vector3 scale = p_transform->getWorldScale();
            offset *= scale;

            scale.rotate(rotation);
            radius *= max(scale.m_x, scale.m_z);
            halfHeight *= scale.m_y;
        }

        m_pxShape->setGeometry(PxCapsuleGeometry(radius, max(halfHeight, radius)));

        const PxTransform pose(reinterpret_cast<const PxVec3&>(offset), reinterpret_cast<const PxQuat&>(rotation));
        m_pxShape->setLocalPose(pose);
    }
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvPhysics::CapsuleCollider& p_component)
    {
        SV_SERVICE(SvPhysics::PhysicsContext).AddCollider(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvPhysics::CapsuleCollider&)
    {
        auto& context = SV_SERVICE(SvPhysics::PhysicsContext);
        context.RemoveCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::CapsuleCollider>() });
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvPhysics::CapsuleCollider& p_component)
    {
        auto& context = SV_SERVICE(SvPhysics::PhysicsContext);
        context.UpdateCollider({ p_entity, ComponentRegistry::GetTypeId<SvPhysics::CapsuleCollider>() }, p_component);
    }
}
