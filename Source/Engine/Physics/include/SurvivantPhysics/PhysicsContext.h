#pragma once
#include "SurvivantPhysics/CollisionInfo.h"
#include "SurvivantPhysics/EPhysicsEvent.h"
#include "SurvivantPhysics/TriggerInfo.h"

#include <SurvivantCore/Events/Event.h>

#include <Vector/Vector3.h>

namespace SvPhysics
{
    class ICollider;
    class RigidBody;
}

namespace physx
{
    class PxRigidDynamic;
    class PxRigidActor;
    class PxMaterial;
    class PxShape;
    class PxDefaultCpuDispatcher;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
}

namespace SvPhysics
{
    class SphereCollider;
    class CapsuleCollider;
    class Material;
    class BoxCollider;

    class PhysicsContext
    {
    public:
        using CollisionEvent = SvCore::Events::Event<EPhysicsEvent, CollisionInfo>;
        using TriggerEvent = SvCore::Events::Event<EPhysicsEvent, TriggerInfo>;

        inline static const LibMath::Vector3 DEFAULT_GRAVITY{ 0.f, -9.807f, 0.f };
        static constexpr uint8_t             DISPATCHER_THREADS_COUNT = 2;

        CollisionEvent m_onCollision;
        TriggerEvent   m_onTrigger;

        /**
         * \brief Creates a default physics context
         */
        PhysicsContext();

        /**
         * \brief Disable physics context copying
         */
        PhysicsContext(const PhysicsContext& p_other) = delete;

        /**
         * \brief Creates a move copy of the given physics context
         * \param p_other The physics context to move
         */
        PhysicsContext(PhysicsContext&& p_other) noexcept = default;

        /**
         * \brief Destroys the physics context
         */
        ~PhysicsContext();

        /**
         * \brief Disable physics context copying
         */
        PhysicsContext& operator=(const PhysicsContext& p_other) = delete;

        /**
         * \brief Moves the given physics context into the current one
         * \param p_other The physics context to move
         * \return The modified physics context
         */
        PhysicsContext& operator=(PhysicsContext&& p_other) noexcept = default;

        /**
         * \brief Initializes the physics context
         * \param p_gravity
         */
        void Init(const LibMath::Vector3& p_gravity = DEFAULT_GRAVITY);

        /**
         * \brief Gets the physics context's gravity
         * \return The context's current gravity
         */
        LibMath::Vector3 GetGravity() const;

        /**
         * \brief Sets the context's gravity
         * \param p_gravity The context's new gravity
         */
        void SetGravity(const LibMath::Vector3& p_gravity);

        /**
         * \brief Adds the given rigid body to the physics context
         * \param p_owner The added rigid body's owner
         * \param p_rigidBody The rigid body to register
         */
        void AddRigidBody(const SvCore::ECS::EntityHandle& p_owner, RigidBody& p_rigidBody);

        /**
         * \brief Updates the rigid body owned by the given entity
         * \param p_owner The updated rigid body's owner
         * \param p_rigidBody The rigid body to update (optional: fetched from the entity handle if nullptr)
         */
        void UpdateRigidBody(SvCore::ECS::EntityHandle p_owner, RigidBody* p_rigidBody);

        /**
         * \brief Removes the rigid body owned by the given entity from the context
         * \param p_owner The removed rigid body's owner
         */
        void RemoveRigidBody(const SvCore::ECS::EntityHandle& p_owner);

        /**
         * \brief Adds the given collider to the physics context
         * \param p_owner The added collider's owner
         * \param p_collider The box collider to register
         */
        void AddCollider(const SvCore::ECS::EntityHandle& p_owner, BoxCollider& p_collider);

        /**
         * \brief Adds the given collider to the physics context
         * \param p_owner The added collider's owner
         * \param p_collider The sphere collider to register
         */
        void AddCollider(const SvCore::ECS::EntityHandle& p_owner, SphereCollider& p_collider);

        /**
         * \brief Adds the given collider to the physics context
         * \param p_owner The added collider's owner
         * \param p_collider The capsule collider to register
         */
        void AddCollider(const SvCore::ECS::EntityHandle& p_owner, CapsuleCollider& p_collider);

        /**
         * \brief Updates the given collider
         * \param p_handle The handle of the collider to update
         * \param p_collider The collider to update
         */
        void UpdateCollider(const SvCore::ECS::ComponentHandle& p_handle, ICollider& p_collider);

        /**
         * \brief Removes the given collider from the physics context
         * \param p_collider The collider to remove
         */
        void RemoveCollider(const SvCore::ECS::ComponentHandle& p_collider);

        /**
         * \brief Updates the physics simulation with the given time step
         * \param p_deltaTime The physics simulation step
         */
        void Update(float p_deltaTime);

        /**
         * \brief Resets the physics context
         */
        void Reset();

        /**
         * \brief Resets and re-initializes the physics context
         */
        void Reload();

    private:
        friend class PhysXEventHandler;
        friend class ICollider;

        std::unordered_map<SvCore::ECS::Entity::Id, physx::PxRigidActor*>   m_entityToActor;
        std::unordered_map<physx::PxRigidActor*, SvCore::ECS::EntityHandle> m_actorToEntity;
        std::unordered_map<physx::PxShape*, SvCore::ECS::ComponentHandle>   m_shapeToCollider;
        std::unordered_map<Material*, physx::PxMaterial*>                   m_materialToPxMat;

        std::unique_ptr<PhysXEventHandler> m_eventHandler;

        physx::PxDefaultCpuDispatcher* m_cpuDispatcher;
        physx::PxFoundation*           m_foundation;
        physx::PxPhysics*              m_physics;
        physx::PxScene*                m_scene;

        /**
         * \brief Moves the PhysX shapes from an actor to another
         * \param p_from The source actor
         * \param p_to The target actor
         */
        static void MoveShapes(physx::PxRigidActor& p_from, physx::PxRigidActor& p_to);

        /**
         * \brief Gets or creates the given entity's corresponding PhysX actor
         * \param p_entity The target entity
         * \param p_canCreate Whether the actor should be created if not found or not
         * \return The actor corresponding to the given entity.
         */
        physx::PxRigidActor* GetActor(SvCore::ECS::EntityHandle p_entity, bool p_canCreate);

        /**
         * \brief Gets or creates the given material's corresponding PhysX material
         * \param p_material The target material
         * \return The PhysX material corresponding to the given material.
         */
        physx::PxMaterial& GetMaterial(Material* p_material);

        /**
         * \brief Gets the collider's corresponding PhysX shape
         * \param p_collider The target collider
         * \return The shape corresponding to the given collider.
         */
        physx::PxShape* GetShape(const SvCore::ECS::ComponentHandle& p_collider);

        /**
         * \brief Refreshes the registered colliders
         */
        void RefreshColliders();

        /**
         * \brief Updates the registered rigid bodies' poses
         */
        void UpdatePoses();
    };
}
