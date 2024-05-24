#pragma once
#include "SurvivantPhysics/EAxisLock.h"
#include "SurvivantPhysics/ECollisionDetectionMode.h"
#include "SurvivantPhysics/EForceMode.h"

#include <SurvivantCore/ECS/ComponentTraits.h>
#include <SurvivantCore/Serialization/MathSerializers.h>

#include <Vector/Vector3.h>

namespace physx
{
    class PxRigidDynamic;
}

namespace SvPhysics
{
    class RigidBody
    {
    public:
        /**
         * \brief Creates a default rigid body
         */
        RigidBody() = default;

        /**
         * \brief Creates a copy of the given rigid body
         * \param p_other The rigid body to copy
         */
        RigidBody(const RigidBody& p_other) = default;

        /**
         * \brief Creates a move copy of the given rigid body
         * \param p_other The rigid body to move
         */
        RigidBody(RigidBody&& p_other) noexcept = default;

        /**
         * \brief Destroys the rigid body
         */
        ~RigidBody() = default;

        /**
         * \brief Assigns a copy of the given rigid body to this one
         * \param p_other The rigid body to copy
         * \return A reference to the modified rigidbody
         */
        RigidBody& operator=(const RigidBody& p_other) = default;

        /**
         * \brief Moves the given rigid body into this one
         * \param p_other The rigid body to move
         * \return A reference to the modified rigidbody
         */
        RigidBody& operator=(RigidBody&& p_other) noexcept = default;

        /**
         * \brief Serializes the rigid body to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the rigid body from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);

        /**
         * \brief Gets the rigid body's mass
         * \return The rigid body's mass
         */
        float GetMass() const;

        /**
         * \brief Sets the rigid body's mass
         * \param p_mass The rigid body's new mass
         */
        void SetMass(float p_mass);

        /**
         * \brief Gets the rigid body's velocity
         * \return The rigid body's velocity
         */
        LibMath::Vector3 GetVelocity() const;

        /**
         * \brief Sets the rigid body's velocity
         * \param p_velocity The rigid body's new velocity
         */
        void SetVelocity(const LibMath::Vector3& p_velocity);

        /**
         * \brief Gets the rigid body's angular velocity
         * \return The rigid body's angular velocity
         */
        LibMath::Vector3 GetAngularVelocity() const;

        /**
         * \brief Sets the rigid body's angular velocity
         * \param p_velocity The rigid body's new angular velocity
         */
        void SetAngularVelocity(const LibMath::Vector3& p_velocity);

        /**
         * \brief Checks whether the rigid body is kinematic or not
         * \return True if the rigid body is kinematic. False otherwise
         */
        bool IsKinematic() const;

        /**
         * \brief Defines whether the rigid body is kinematic or not
         * \param p_isKinematic Whether the rigid body is kinematic or not
         */
        void SetKinematic(bool p_isKinematic);

        /**
         * \brief Checks whether the rigid body is affected by gravity or not
         * \return True if the rigid body is affected by gravity. False otherwise
         */
        bool CanUseGravity() const;

        /**
         * \brief Defines whether the rigid body is affected by gravity or not
         * \param p_useGravity Whether the rigid body is affected by gravity or not
         */
        void SetUseGravity(bool p_useGravity);

        /**
         * \brief Gets the rigid body's axis locks
         * \return The rigid body's axis locks
         */
        EAxisLockFlags GetAxisLocks() const;

        /**
         * \brief Sets the rigid body's axis locks
         * \param p_mode The rigid body's new axis locks
         */
        void SetAxisLocks(EAxisLockFlags p_mode);

        /**
         * \brief Gets the rigid body's collision detection mode
         * \return The rigid body's collision detection mode
         */
        ECollisionDetectionMode GetCollisionDetectionMode() const;

        /**
         * \brief Sets the rigid body's collision detection mode
         * \param p_mode The rigid body's new collision detection mode
         */
        void SetCollisionDetectionMode(ECollisionDetectionMode p_mode);

        /**
         * \brief Checks whether the rigid body is currently sleeping
         * \return True if the rigid body is currently sleeping. False otherwise
         */
        bool IsSleeping() const;

        /**
         * \brief Forces the rigid body to sleep
         */
        void Sleep();

        /**
         * \brief Forces the rigid body to wake up
         */
        void WakeUp();

        /**
         * \brief Adds the given force to the rigid body
         * \param p_force The force to add
         * \param p_mode The force mode
         * \param p_autoWake Whether the call should wake up the rigid body if it is currently sleeping
         */
        void AddForce(const LibMath::Vector3& p_force, EForceMode p_mode = EForceMode::FORCE, bool p_autoWake = true);

        /**
         * \brief Adds the given torque to the rigid body
         * \param p_torque The torque to add
         * \param p_mode The force mode
         * \param p_autoWake Whether the call should wake up the rigid body if it is currently sleeping
         */
        void AddTorque(const LibMath::Vector3& p_torque, EForceMode p_mode = EForceMode::FORCE, bool p_autoWake = true);

    private:
        friend class PhysicsContext;

        float m_mass = 1.f;

        bool m_isKinematic = false;
        bool m_useGravity  = true;

        EAxisLockFlags m_axisLocks = EAxisLock::NONE;

        ECollisionDetectionMode m_collisionDetectionMode = ECollisionDetectionMode::DISCRETE;

        physx::PxRigidDynamic* m_pxBody;

        /**
         * \brief Updates the rigid body's internal PhysX actor
         */
        void Refresh();
    };
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle&, SvPhysics::RigidBody&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, SvPhysics::RigidBody&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, SvPhysics::RigidBody&);
}
