#include "SurvivantPhysics/RigidBody.h"

#include "SurvivantPhysics/PhysicsContext.h"

#include <SurvivantCore/ECS/Components/Hierarchy.h> // Include this instead of transform to avoid redefinition of Transform Traits

#include <PxRigidDynamic.h>

using namespace physx;

namespace SvPhysics
{
    // Ensure our lock flags are compatible with PhysX lock flags
    static_assert(
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::X_POSITION) == PxRigidDynamicLockFlag::eLOCK_LINEAR_X &&
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::Y_POSITION) == PxRigidDynamicLockFlag::eLOCK_LINEAR_Y &&
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::Z_POSITION) == PxRigidDynamicLockFlag::eLOCK_LINEAR_Z &&
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::X_ROTATION) == PxRigidDynamicLockFlag::eLOCK_ANGULAR_X &&
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::Y_ROTATION) == PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y &&
        static_cast<PxRigidDynamicLockFlag::Enum>(EAxisLock::Z_ROTATION) == PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z
    );

    PxForceMode::Enum ToPxForceMode(const EForceMode p_mode)
    {
        switch (p_mode)
        {
        case EForceMode::FORCE:
            return PxForceMode::eFORCE;
        case EForceMode::IMPULSE:
            return PxForceMode::eIMPULSE;
        case EForceMode::ACCELERATION:
            return PxForceMode::eACCELERATION;
        case EForceMode::VELOCITY_CHANGE:
            return PxForceMode::eVELOCITY_CHANGE;
        default:
            ASSERT(false, "Invalid force mode");
            return PxForceMode::eFORCE;
        }
    }

    bool RigidBody::ToJson(SvCore::Serialization::JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("mass");
        if (!CHECK(p_writer.Double(m_mass), "Failed to write rigid body mass"))
            return false;

        p_writer.Key("is_kinematic");
        if (!CHECK(p_writer.Bool(m_isKinematic), "Failed to write rigid body is kinematic flag"))
            return false;

        p_writer.Key("use_gravity");
        if (!CHECK(p_writer.Bool(m_useGravity), "Failed to write rigid body use gravity flag"))
            return false;

        p_writer.Key("axis_locks");
        if (!CHECK(p_writer.Uint(m_axisLocks), "Failed to write rigid body axis locks"))
            return false;

        p_writer.Key("collision_mode");
        if (!CHECK(p_writer.Uint(static_cast<uint8_t>(m_collisionDetectionMode)), "Failed to write rigid body collision mode"))
            return false;

        return CHECK(p_writer.EndObject(), "Failed to serialize rigid body");
    }

    bool RigidBody::FromJson(const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize rigid body - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("mass");
        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize rigid body - missing mass"))
            return false;

        if (!CHECK(it->value.IsFloat(), "Unable to deserialize rigid body mass - Json value should be a float"))
            return false;

        m_mass = it->value.GetFloat();

        it = p_json.FindMember("is_kinematic");
        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize rigid body - missing is kinematic flag"))
            return false;

        if (!CHECK(it->value.IsBool(), "Unable to deserialize rigid body is kinematic flag - Json value should be a boolean"))
            return false;

        m_isKinematic = it->value.GetBool();

        it = p_json.FindMember("use_gravity");
        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize rigid body - missing use gravity flag"))
            return false;

        if (!CHECK(it->value.IsBool(), "Unable to deserialize rigid body use gravity flag - Json value should be a boolean"))
            return false;

        m_useGravity = it->value.GetBool();

        it = p_json.FindMember("axis_locks");
        if (it != p_json.MemberEnd())
        {
            if (!CHECK(it->value.IsUint(), "Unable to deserialize rigid body axis locks - Json value should be a uint"))
                return false;

            m_axisLocks = static_cast<EAxisLockFlags::DataType>(it->value.GetUint());
        }
        else
        {
            m_axisLocks = EAxisLock::NONE;
        }

        it = p_json.FindMember("collision_mode");
        if (it != p_json.MemberEnd())
        {
            if (!CHECK(it->value.IsUint(), "Unable to deserialize rigid body collision mode - Json value should be a uint"))
                return false;

            m_collisionDetectionMode = static_cast<ECollisionDetectionMode>(it->value.GetUint());
        }
        else
        {
            m_collisionDetectionMode = ECollisionDetectionMode::DISCRETE;
        }

        return true;
    }

    float RigidBody::GetMass() const
    {
        return m_mass;
    }

    void RigidBody::SetMass(const float p_mass)
    {
        m_mass = p_mass;
        m_pxBody->setMass(m_mass);
    }

    LibMath::Vector3 RigidBody::GetVelocity() const
    {
        PxVec3 velocity = m_pxBody->getLinearVelocity();
        return { velocity.x, velocity.y, velocity.z };
    }

    void RigidBody::SetVelocity(const LibMath::Vector3& p_velocity)
    {
        m_pxBody->setLinearVelocity(reinterpret_cast<const PxVec3&>(p_velocity));
    }

    LibMath::Vector3 RigidBody::GetAngularVelocity() const
    {
        PxVec3 velocity = m_pxBody->getAngularVelocity();
        return { velocity.x, velocity.y, velocity.z };
    }

    void RigidBody::SetAngularVelocity(const LibMath::Vector3& p_velocity)
    {
        m_pxBody->setAngularVelocity(reinterpret_cast<const PxVec3&>(p_velocity));
    }

    bool RigidBody::IsKinematic() const
    {
        return m_isKinematic;
    }

    void RigidBody::SetKinematic(const bool p_isKinematic)
    {
        m_isKinematic = p_isKinematic;
        m_pxBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_isKinematic);
    }

    bool RigidBody::CanUseGravity() const
    {
        return m_useGravity;
    }

    void RigidBody::SetUseGravity(const bool p_useGravity)
    {
        m_useGravity = p_useGravity;
        m_pxBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_useGravity);
    }

    EAxisLockFlags RigidBody::GetAxisLocks() const
    {
        return m_axisLocks;
    }

    void RigidBody::SetAxisLocks(const EAxisLockFlags p_mode)
    {
        m_axisLocks = p_mode;
        m_pxBody->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlags>(p_mode));
    }

    ECollisionDetectionMode RigidBody::GetCollisionDetectionMode() const
    {
        return m_collisionDetectionMode;
    }

    void RigidBody::SetCollisionDetectionMode(const ECollisionDetectionMode p_mode)
    {
        const PxRigidBodyFlags flags = m_pxBody->getRigidBodyFlags()
            & ~(PxRigidBodyFlag::eENABLE_CCD | PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD);

        switch (m_collisionDetectionMode = p_mode)
        {
        case ECollisionDetectionMode::DISCRETE:
            m_pxBody->setRigidBodyFlags(flags);
            break;
        case ECollisionDetectionMode::CONTINUOUS:
            m_pxBody->setRigidBodyFlags(flags | PxRigidBodyFlag::eENABLE_CCD);
            break;
        case ECollisionDetectionMode::CONTINUOUS_SPECULATIVE:
            m_pxBody->setRigidBodyFlags(flags | PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD);
            break;
        }
    }

    bool RigidBody::IsSleeping() const
    {
        return m_pxBody->isSleeping();
    }

    void RigidBody::Sleep()
    {
        m_pxBody->putToSleep();
    }

    void RigidBody::WakeUp()
    {
        m_pxBody->wakeUp();
    }

    void RigidBody::AddForce(const LibMath::Vector3& p_force, const EForceMode p_mode, const bool p_autoWake)
    {
        m_pxBody->addForce(reinterpret_cast<const PxVec3&>(p_force), ToPxForceMode(p_mode), p_autoWake);
    }

    void RigidBody::AddTorque(const LibMath::Vector3& p_torque, const EForceMode p_mode, const bool p_autoWake)
    {
        m_pxBody->addTorque(reinterpret_cast<const PxVec3&>(p_torque), ToPxForceMode(p_mode), p_autoWake);
    }

    void RigidBody::Refresh()
    {
        SetMass(m_mass);
        SetKinematic(m_isKinematic);
        SetUseGravity(m_useGravity);
        SetCollisionDetectionMode(m_collisionDetectionMode);
        SetAxisLocks(m_axisLocks);
    }
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvPhysics::RigidBody& p_component)
    {
        if (!p_entity.Has<LibMath::Transform>())
            p_entity.Make<LibMath::Transform>();

        SvPhysics::PhysicsContext::GetInstance().AddRigidBody(p_entity, p_component);
    }

    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvPhysics::RigidBody&)
    {
        SvPhysics::PhysicsContext::GetInstance().RemoveRigidBody(p_entity);
    }

    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvPhysics::RigidBody& p_component)
    {
        SvPhysics::PhysicsContext::GetInstance().UpdateRigidBody(p_entity, &p_component);
    }
}
