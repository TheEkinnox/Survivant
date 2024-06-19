#include "SurvivantPhysics/PhysicsContext.h"

#include "SurvivantPhysics/RigidBody.h"
#include "SurvivantPhysics/Collider/BoxCollider.h"
#include "SurvivantPhysics/Collider/CapsuleCollider.h"
#include "SurvivantPhysics/Collider/SphereCollider.h"

#include <SurvivantCore/Debug/Assertion.h>

#include <PxConfig.h>
#include <PxMaterial.h>
#include <PxPhysics.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxScene.h>
#include <PxSimulationEventCallback.h>
#include <Transform.h>

#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>

#include <foundation/PxPhysicsVersion.h>

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvCore::Debug;
using namespace SvCore::Utility;
using namespace physx;

namespace SvPhysics
{
    // Ensure the LibMath and PhysX math classes are equivalent
    static_assert(
        sizeof(PxVec3) == sizeof(Vector3) &&
        offsetof(PxVec3, x) == offsetof(Vector3, m_x) &&
        offsetof(PxVec3, y) == offsetof(Vector3, m_y) &&
        offsetof(PxVec3, z) == offsetof(Vector3, m_z)
    );

    static_assert(
        sizeof(PxQuat) == sizeof(Quaternion) &&
        offsetof(PxQuat, x) == offsetof(Quaternion, m_x) &&
        offsetof(PxQuat, y) == offsetof(Quaternion, m_y) &&
        offsetof(PxQuat, z) == offsetof(Quaternion, m_z) &&
        offsetof(PxQuat, w) == offsetof(Quaternion, m_w)
    );

    namespace
    {
        PxFilterFlags PhysXFilterShader(
            const PxFilterObjectAttributes p_attributes0,
            const PxFilterData             p_filterData0,
            const PxFilterObjectAttributes p_attributes1,
            const PxFilterData             p_filterData1,
            PxPairFlags&                   p_pairFlags,
            const void*                    p_constantBlock,
            const PxU32                    p_constantBlockSize)
        {
            auto tmp = PxDefaultSimulationFilterShader(
                p_attributes0, p_filterData0,
                p_attributes1, p_filterData1,
                p_pairFlags, p_constantBlock, p_constantBlockSize
            );

            if (tmp == PxFilterFlag::eSUPPRESS)
                return tmp;

            if (!(PxFilterObjectIsTrigger(p_attributes0) || PxFilterObjectIsTrigger(p_attributes1)))
                p_pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eNOTIFY_CONTACT_POINTS
                    | PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

            p_pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
            return tmp;
        }

        PxTransform GetPose(const Transform* p_transform)
        {
            const Vector3    position = p_transform ? p_transform->getWorldPosition() : Vector3::zero();
            const Quaternion rotation = p_transform ? p_transform->getWorldRotation().normalized() : Quaternion::identity();

            return { reinterpret_cast<const PxVec3&>(position), reinterpret_cast<const PxQuat&>(rotation) };
        }
    }

    class PhysXErrorHandler final : public PxErrorCallback
    {
    public:
        void reportError(const PxErrorCode::Enum p_code, const char* p_message, const char* p_file, const int p_line) override
        {
            std::string format;
            ELogType    logType;

            switch (p_code)
            {
            case PxErrorCode::eNO_ERROR:
                return;
            case PxErrorCode::eDEBUG_INFO:
            {
                format  = "[PhysX] %s";
                logType = ELogType::DEBUG_LOG;
                break;
            }
            case PxErrorCode::eDEBUG_WARNING:
            {
                format  = "[PhysX] %s";
                logType = ELogType::WARNING_LOG;
                break;
            }
            case PxErrorCode::eINVALID_PARAMETER:
            {
                format  = "[PhysX] [Invalid Parameter] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            case PxErrorCode::eINVALID_OPERATION:
            {
                format  = "[PhysX] [Invalid Operation] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            case PxErrorCode::eOUT_OF_MEMORY:
            {
                format  = "[PhysX] [Out Of Memory] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            case PxErrorCode::eINTERNAL_ERROR:
            {
                format  = "[PhysX] [Internal Error] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            case PxErrorCode::eABORT:
            {
                format  = "[PhysX] [Fatal Error] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            case PxErrorCode::ePERF_WARNING:
            {
                format  = "[PhysX] [Performance] %s";
                logType = ELogType::WARNING_LOG;
                break;
            }
            case PxErrorCode::eMASK_ALL:
            default:
            {
                format  = "[PhysX] [Unknown Error] %s";
                logType = ELogType::ERROR_LOG;
                break;
            }
            }

            Logger::GetInstance().DebugLog(p_file, p_line, format.c_str(), logType, p_message);
            ASSERT(p_code != physx::PxErrorCode::eABORT);
        }
    };

    class PhysXEventHandler final : public PxSimulationEventCallback
    {
    public:
        explicit PhysXEventHandler(PhysicsContext& p_context)
            : m_context(&p_context)
        {
        }

        void onConstraintBreak(PxConstraintInfo* /*p_constraints*/, const PxU32 /*p_count*/) override
        {
        }

        void onWake(PxActor** /*p_actors*/, const PxU32 /*p_count*/) override
        {
        }

        void onSleep(PxActor** /*p_actors*/, const PxU32 /*p_count*/) override
        {
        }

        void onContact(const PxContactPairHeader& p_pairHeader, const PxContactPair* p_pairs, PxU32 p_count) override
        {
            if (p_pairHeader.flags & (PxContactPairHeaderFlag::eREMOVED_ACTOR_0 | PxContactPairHeaderFlag::eREMOVED_ACTOR_1))
                return;

            const auto& shapeToCollider = m_context->m_shapeToCollider;
            auto&       collisionEvent  = m_context->m_onCollision;

            while (p_count--)
            {
                const PxContactPair& pair = *(p_pairs++);

                if (pair.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
                    continue;

                EPhysicsEvent eventType = EPhysicsEvent::NONE;

                if (pair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                    eventType = EPhysicsEvent::ENTER;
                else if (pair.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
                    eventType = EPhysicsEvent::STAY;
                else if (pair.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
                    eventType = EPhysicsEvent::EXIT;

                ASSERT(eventType != EPhysicsEvent::NONE, "Unsupported PhysX collision event");

                CollisionInfo info;

                auto it = shapeToCollider.find(pair.shapes[0]);
                ASSERT(it != shapeToCollider.end(), "Received collision event with unregistered PhysX shape 0");
                info.m_colliders[0] = it->second;

                it = shapeToCollider.find(pair.shapes[1]);
                ASSERT(it != shapeToCollider.end(), "Received collision event with unregistered PhysX shape 1");
                info.m_colliders[1] = it->second;

                ProcessContacts(info, pair);
                collisionEvent.Invoke(eventType, info);
            }
        }

        void onTrigger(PxTriggerPair* p_pairs, PxU32 p_count) override
        {
            const auto& shapeToCollider = m_context->m_shapeToCollider;
            auto&       triggerEvent    = m_context->m_onTrigger;

            while (p_count--)
            {
                const PxTriggerPair& pair = *(p_pairs++);

                if (pair.flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
                    continue;

                EPhysicsEvent eventType = EPhysicsEvent::NONE;

                if (pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                    eventType = EPhysicsEvent::ENTER;
                else if (pair.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
                    eventType = EPhysicsEvent::EXIT;

                ASSERT(eventType != EPhysicsEvent::NONE, "Unsupported PhysX trigger status");

                TriggerInfo info;

                auto it = shapeToCollider.find(pair.triggerShape);
                ASSERT(it != shapeToCollider.end(), "Received trigger event from unregistered PhysX shape");
                info.m_triggerCollider = it->second;

                it = shapeToCollider.find(pair.otherShape);
                ASSERT(it != shapeToCollider.end(), "Received trigger event with unregistered PhysX shape");
                info.m_otherCollider = it->second;

                triggerEvent.Invoke(eventType, info);
            }
        }

        void onAdvance(const PxRigidBody* const* p_bodyBuffer, const PxTransform* p_poseBuffer, PxU32 p_count) override
        {
            auto& actorToEntity = m_context->m_actorToEntity;

            while (p_count--)
            {
                const PxRigidBody* rb   = *(p_bodyBuffer++);
                const PxTransform& pose = *(p_poseBuffer++);

                const auto it = actorToEntity.find(const_cast<PxRigidBody*>(rb));
                ASSERT(it != actorToEntity.end(), "Received advance event from PhysX rigid body created by another context");

                Transform* transform = it->second.Get<Transform>();
                ASSERT(transform, "Rigid bodies must have a transform");

                const Vector3&   position = reinterpret_cast<const Vector3&>(pose.p);
                const Quaternion rotation = reinterpret_cast<const Quaternion&>(pose.q);

                transform->setAllWorld(position, rotation, transform->getWorldScale());
            }
        }

    private:
        PhysicsContext* m_context;

        static void ProcessContacts(CollisionInfo& p_info, const PxContactPair& p_pair)
        {
            auto* contactPoints = static_cast<PxContactPairPoint*>(alloca(sizeof(PxContactPairPoint) * p_pair.contactCount));
            p_pair.extractContacts(contactPoints, p_pair.contactCount);

            p_info.m_contacts.reserve(p_pair.contactCount);

            for (PxU8 i = 0; i < p_pair.contactCount; ++i)
            {
                const auto& [position, separation, normal, _0, impulse, _1] = contactPoints[i];

                p_info.m_contacts.emplace_back(
                    Vector3(position.x, position.y, position.z),
                    Vector3(impulse.x, impulse.y, impulse.z),
                    Vector3(normal.x, normal.y, normal.z),
                    separation
                );
            }
        }
    };

    PhysicsContext::PhysicsContext()
        : m_scene(nullptr)
    {
        static PxDefaultAllocator defaultAllocator;
        static PhysXErrorHandler  errorHandler;

        m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator, errorHandler);
        ASSERT(m_foundation, "Failed to create PhysX foundation");

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale());
        ASSERT(m_physics, "Failed to create PhysX physics object");

        m_cpuDispatcher = PxDefaultCpuDispatcherCreate(DISPATCHER_THREADS_COUNT);
        ASSERT(m_cpuDispatcher, "Failed to create PhysX CPU dispatcher with %u threads", DISPATCHER_THREADS_COUNT);

        m_eventHandler = std::make_unique<PhysXEventHandler>(*this);
    }

    PhysicsContext::~PhysicsContext()
    {
        Reset();

        PX_RELEASE(m_cpuDispatcher)
        PX_RELEASE(m_physics)
        PX_RELEASE(m_foundation)
        m_eventHandler.reset();
        m_materialToPxMat.clear();
    }

    void PhysicsContext::Init(const Vector3& p_gravity)
    {
        if (ASSUME_FALSE(m_scene, "Attempted to initialize already initialzed physics context"))
            return;

        PxSceneDesc sceneDesc(m_physics->getTolerancesScale());

        sceneDesc.gravity                 = { p_gravity.m_x, p_gravity.m_y, p_gravity.m_z };
        sceneDesc.cpuDispatcher           = m_cpuDispatcher;
        sceneDesc.filterShader            = PhysXFilterShader;
        sceneDesc.simulationEventCallback = m_eventHandler.get();
        sceneDesc.flags                   = sceneDesc.flags | PxSceneFlag::eENABLE_CCD;

        m_scene = m_physics->createScene(sceneDesc);
        ASSERT(m_scene, "Failed to create PhysX scene");
    }

    Vector3 PhysicsContext::GetGravity() const
    {
        ASSERT(m_scene, "Attempted to get gravity of uninitialized physics context");
        PxVec3 gravity = m_scene->getGravity();
        return { gravity.x, gravity.y, gravity.z };
    }

    void PhysicsContext::SetGravity(const Vector3& p_gravity)
    {
        ASSERT(m_scene, "Attempted to set gravity of uninitialized physics context");
        m_scene->setGravity(reinterpret_cast<const PxVec3&>(p_gravity));
    }

    void PhysicsContext::AddRigidBody(const EntityHandle& p_owner, RigidBody& p_rigidBody)
    {
        if (!p_owner)
            return;

        PxRigidActor* actor = GetActor(p_owner, false);
        ASSERT(!actor || !actor->is<PxRigidDynamic>(), "Attempted to register rigid body more than once.");
        ASSERT(p_owner.Has<Transform>(), "Rigid bodies must have a transform");

        PxRigidDynamic* pxRigidBody = m_physics->createRigidDynamic(GetPose(p_owner.Get<Transform>()));
        ASSERT(pxRigidBody, "Failed to create PhysX rigid body");

        pxRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_POSE_INTEGRATION_PREVIEW, true);
        CHECK(m_scene->addActor(*pxRigidBody), "Failed to add rigid body to PhysX scene");
        p_rigidBody.m_pxBody = pxRigidBody;
        p_rigidBody.Refresh();

        if (actor)
        {
            MoveShapes(*actor, *pxRigidBody);

            m_scene->removeActor(*actor);
            m_actorToEntity.erase(actor);
            actor->release();
        }

        m_actorToEntity[pxRigidBody]         = p_owner;
        m_entityToActor[p_owner.GetEntity()] = pxRigidBody;
    }

    void PhysicsContext::UpdateRigidBody(EntityHandle p_owner, RigidBody* p_rigidBody)
    {
        PxRigidDynamic* actor = GetActor(p_owner, false)->is<PxRigidDynamic>();
        ASSERT(actor, "Attempted to update invalid rigid body");

        if (!p_rigidBody)
            p_rigidBody = p_owner.Get<RigidBody>();

        ASSERT(p_rigidBody, "Attempted to update null rigid body");

        if (p_rigidBody->m_pxBody && p_rigidBody->m_pxBody != actor)
        {
            MoveShapes(*p_rigidBody->m_pxBody, *actor);

            m_scene->removeActor(*p_rigidBody->m_pxBody);
            m_actorToEntity.erase(p_rigidBody->m_pxBody);
            p_rigidBody->m_pxBody->release();
        }

        p_rigidBody->m_pxBody = actor;
        p_rigidBody->Refresh();
    }

    void PhysicsContext::RemoveRigidBody(const EntityHandle& p_owner)
    {
        const auto it = m_entityToActor.find(p_owner.GetEntity());

        if (it == m_entityToActor.end())
            return;

        PxRigidActor& removedActor = *it->second;

        if (!removedActor.is<PxRigidBody>() && removedActor.getNbShapes() > 0)
        {
            PxRigidStatic* pxRigidActor = m_physics->createRigidStatic(removedActor.getGlobalPose());
            ASSERT(pxRigidActor, "Failed to create PhysX static rigid body");

            CHECK(m_scene->addActor(*pxRigidActor), "Failed to add static rigid body to PhysX scene");

            MoveShapes(removedActor, *pxRigidActor);
            m_actorToEntity[pxRigidActor] = p_owner;

            it->second = pxRigidActor;
        }
        else
        {
            m_entityToActor.erase(it);
        }

        m_actorToEntity.erase(&removedActor);
        m_scene->removeActor(removedActor);
        removedActor.release();
    }

    void PhysicsContext::AddCollider(const EntityHandle& p_owner, BoxCollider& p_collider)
    {
        const PxMaterial& mat = GetMaterial(p_collider.m_material.GetOrDefault());

        PxShape* pxBox = m_physics->createShape(PxBoxGeometry(PxVec3(.5f)), mat, true);
        ASSERT(pxBox, "Failed to create PhysX box shape");

        p_collider.m_pxShape = pxBox;
        static_cast<ICollider&>(p_collider).Refresh(p_owner.Get<Transform>());

        PxRigidActor* actor = GetActor(p_owner, true);
        CHECK(actor->attachShape(*pxBox), "Failed to attach box collider to PhysX actor");
        pxBox->release();

        m_shapeToCollider[pxBox] = { p_owner, ComponentRegistry::GetTypeId<BoxCollider>() };
    }

    void PhysicsContext::AddCollider(const EntityHandle& p_owner, SphereCollider& p_collider)
    {
        PxShape* pxSphere = m_physics->createShape(PxSphereGeometry(.5f), GetMaterial(p_collider.m_material.GetOrDefault()), true);
        ASSERT(pxSphere, "Failed to create PhysX sphere shape");

        p_collider.m_pxShape = pxSphere;
        static_cast<ICollider&>(p_collider).Refresh(p_owner.Get<Transform>());

        PxRigidActor* actor = GetActor(p_owner, true);
        CHECK(actor->attachShape(*pxSphere), "Failed to attach sphere collider to PhysX actor");
        pxSphere->release();

        m_shapeToCollider[pxSphere] = { p_owner, ComponentRegistry::GetTypeId<SphereCollider>() };
    }

    void PhysicsContext::AddCollider(const EntityHandle& p_owner, CapsuleCollider& p_collider)
    {
        const PxMaterial& mat = GetMaterial(p_collider.m_material.GetOrDefault());

        PxShape* pxCapsule = m_physics->createShape(PxCapsuleGeometry(.5f, .5f), mat, true);
        ASSERT(pxCapsule, "Failed to create PhysX capsule shape");

        p_collider.m_pxShape = pxCapsule;
        static_cast<ICollider&>(p_collider).Refresh(p_owner.Get<Transform>());

        PxRigidActor* actor = GetActor(p_owner, true);
        CHECK(actor->attachShape(*pxCapsule), "Failed to attach capsule collider to PhysX actor");
        pxCapsule->release();

        m_shapeToCollider[pxCapsule] = { p_owner, ComponentRegistry::GetTypeId<CapsuleCollider>() };
    }

    void PhysicsContext::UpdateCollider(const ComponentHandle& p_handle, ICollider& p_collider)
    {
        PxShape* pxShape = GetShape(p_handle);
        ASSERT(pxShape, "Attempted to update collider without linked PhysX shape");

        p_collider.m_pxShape = pxShape;
        p_collider.Refresh(p_handle.m_owner.Get<Transform>());
    }

    void PhysicsContext::RemoveCollider(const ComponentHandle& p_collider)
    {
        if (!p_collider)
            return;

        auto toRemove = m_shapeToCollider.begin();

        while (toRemove != m_shapeToCollider.end())
        {
            if (toRemove->second == p_collider)
                break;

            ++toRemove;
        }

        if (toRemove == m_shapeToCollider.end())
            return;

        if (PxRigidActor* actor = GetActor(p_collider.m_owner, false))
        {
            actor->detachShape(*toRemove->first);

            if (actor->is<PxRigidStatic>() && actor->getNbShapes() == 0)
            {
                m_scene->removeActor(*actor);
                m_actorToEntity.erase(actor);
                actor->release();

                m_entityToActor.erase(p_collider.m_owner.GetEntity());
            }
        }

        m_shapeToCollider.erase(toRemove);
    }

    void PhysicsContext::Update(float p_deltaTime)
    {
        if (!ASSUME(m_scene, "Attempted to update uninitialized physics context"))
            return;

        [[unlikely]] if (floatEquals(p_deltaTime, 0.f)) // Ignore first frame
            return;

        RefreshColliders();
        UpdatePoses();

        while (p_deltaTime > 0.f)
        {
            // The simulation becomes unstable when the time step is too large
            // Split it in smaller calls in this situation
            constexpr float maxTimeStep = .05f;

            const float timeStep = min(p_deltaTime, maxTimeStep);

            if (!CHECK(m_scene->simulate(timeStep), "Failed to simulate physics"))
                return;

            CHECK(m_scene->fetchResults(true), "Failed to fetch physics simulation results");

            p_deltaTime -= timeStep;
        }
    }

    void PhysicsContext::Reset()
    {
        if (m_scene)
        {
            m_scene->flushSimulation(true);
            m_scene->release();
            m_scene = nullptr;
        }

        for (const auto& actor : m_entityToActor | std::views::values)
        {
            if (actor)
                actor->release();
        }

        m_entityToActor.clear();
        m_actorToEntity.clear();
        m_shapeToCollider.clear();
    }

    void PhysicsContext::Reload()
    {
        Reset();
        Init();
    }

    void PhysicsContext::MoveShapes(PxRigidActor& p_from, PxRigidActor& p_to)
    {
        if (&p_from == &p_to)
            return;

        for (PxU32 i = 0; i < p_from.getNbShapes(); ++i)
        {
            PxShape* shape;
            p_from.getShapes(&shape, 1, i);

            shape->acquireReference(); // Required for the shape to not get destroyed before being attached to its new owner
            p_from.detachShape(*shape); // Exclusive shapes can only be attached to one actor at a time.

            p_to.attachShape(*shape);
            shape->release();
        }
    }

    PxRigidActor* PhysicsContext::GetActor(EntityHandle p_entity, const bool p_canCreate)
    {
        if (!p_entity)
            return nullptr;

        const auto    it    = m_entityToActor.find(p_entity.GetEntity());
        PxRigidActor* actor = it != m_entityToActor.end() ? it->second : nullptr;

        if (actor || !p_canCreate)
            return actor;

        if (RigidBody* rigidBody = p_entity.Get<RigidBody>())
        {
            AddRigidBody(p_entity, *rigidBody);
            return rigidBody->m_pxBody;
        }

        PxRigidStatic* pxRigidActor = m_physics->createRigidStatic(GetPose(p_entity.Get<Transform>()));
        ASSERT(pxRigidActor, "Failed to create PhysX static rigid body");

        CHECK(m_scene->addActor(*pxRigidActor), "Failed to add static rigid body to PhysX scene");

        m_actorToEntity[pxRigidActor] = p_entity;
        return m_entityToActor[p_entity.GetEntity()] = pxRigidActor;
    }

    PxMaterial& PhysicsContext::GetMaterial(Material* p_material)
    {
        if (!p_material)
        {
            static const Material defaultMat;
            static PxMaterial*    defaultPxMat = m_physics->createMaterial(
                defaultMat.m_staticFriction, defaultMat.m_dynamicFriction, defaultMat.m_restitution);

            ASSERT(defaultPxMat, "Failed to create default PhysX material");
            return *defaultPxMat;
        }

        const auto  it    = m_materialToPxMat.find(p_material);
        PxMaterial* pxMat = it != m_materialToPxMat.end() ? it->second : nullptr;

        if (pxMat)
        {
            pxMat->setStaticFriction(p_material->m_staticFriction);
            pxMat->setDynamicFriction(p_material->m_dynamicFriction);
            pxMat->setRestitution(p_material->m_restitution);
            return *pxMat;
        }

        pxMat = m_physics->createMaterial(p_material->m_staticFriction, p_material->m_dynamicFriction, p_material->m_restitution);
        ASSERT(pxMat, "Failed to create PhysX material");

        return *(m_materialToPxMat[p_material] = pxMat);
    }

    PxShape* PhysicsContext::GetShape(const ComponentHandle& p_collider)
    {
        auto shape = m_shapeToCollider.begin();

        while (shape != m_shapeToCollider.end())
        {
            if (shape->second == p_collider)
                return shape->first;

            ++shape;
        }

        return nullptr;
    }

    void PhysicsContext::RefreshColliders()
    {
        for (auto& [shape, colliderHandle] : m_shapeToCollider)
        {
            if (!shape)
                continue;

            ICollider* collider = colliderHandle.Get<ICollider>();
            ASSERT(collider, "Attempted to refresh an invalid collider");
            UpdateCollider(colliderHandle, *collider);
        }
    }

    void PhysicsContext::UpdatePoses()
    {
        for (auto& [actor, entity] : m_actorToEntity)
        {
            PxRigidBody* rb = actor->is<PxRigidBody>();

            if (!rb)
                continue;

            ASSERT(entity.Has<Transform>(), "Rigid bodies must have a transform");
            rb->setGlobalPose(GetPose(entity.Get<Transform>()));
        }
    }
}
