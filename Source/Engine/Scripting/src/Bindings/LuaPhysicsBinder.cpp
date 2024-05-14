#include "SurvivantScripting/Bindings/LuaPhysicsBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantPhysics/CollisionInfo.h>
#include <SurvivantPhysics/Material.h>
#include <SurvivantPhysics/RigidBody.h>
#include <SurvivantPhysics/TriggerInfo.h>

using namespace SvPhysics;

namespace SvScripting::Bindings
{
    void LuaPhysicsBinder::Bind(sol::state& p_luaState)
    {
        BindContext(p_luaState);
        BindRigidBody(p_luaState);
        BindCollisionInfo(p_luaState);
        BindTriggerInfo(p_luaState);
        BindForceModes(p_luaState);
        BindCollisionDetectionModes(p_luaState);
        BindContact(p_luaState);
        BindMaterial(p_luaState);
        BindICollider(p_luaState);
        BindBoxCollider(p_luaState);
        BindSphereCollider(p_luaState);
        BindCapsuleCollider(p_luaState);
    }

    void LuaPhysicsBinder::BindContext(sol::state& /*p_luaState*/)
    {
        // TODO: Physics context lua bindings
    }

    void LuaPhysicsBinder::BindRigidBody(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "RigidBody";

        sol::usertype resourceType = p_luaState.new_usertype<RigidBody>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "mass", sol::property(&RigidBody::GetMass, &RigidBody::SetMass),
            "velocity", sol::property(&RigidBody::GetVelocity, &RigidBody::SetVelocity),
            "angularVelocity", sol::property(&RigidBody::GetAngularVelocity, &RigidBody::SetAngularVelocity),
            "isKinematic", sol::property(&RigidBody::IsKinematic, &RigidBody::SetKinematic),
            "useGravity", sol::property(&RigidBody::CanUseGravity, &RigidBody::SetUseGravity),
            "collisionDetectionMode", sol::property(&RigidBody::GetCollisionDetectionMode, &RigidBody::SetCollisionDetectionMode),
            "isSleeping", sol::property(&RigidBody::IsSleeping, [](RigidBody& p_self, const bool p_isSleeping)
            {
                p_isSleeping ? p_self.Sleep() : p_self.WakeUp();
            }),
            "Sleep", &RigidBody::Sleep,
            "WakeUp", &RigidBody::WakeUp,
            "AddForce", sol::overload(
                &RigidBody::AddForce,
                [](RigidBody& p_self, const LibMath::Vector3& p_force, const EForceMode p_mode)
                {
                    p_self.AddForce(p_force, p_mode);
                },
                [](RigidBody& p_self, const LibMath::Vector3& p_force)
                {
                    p_self.AddForce(p_force);
                }
            ),
            "AddTorque", sol::overload(
                &RigidBody::AddTorque,
                [](RigidBody& p_self, const LibMath::Vector3& p_torque, const EForceMode p_mode)
                {
                    p_self.AddTorque(p_torque, p_mode);
                },
                [](RigidBody& p_self, const LibMath::Vector3& p_torque)
                {
                    p_self.AddTorque(p_torque);
                }
            )
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<RigidBody>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindCollisionInfo(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "CollisionInfo";

        sol::usertype resourceType = p_luaState.new_usertype<CollisionInfo>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "colliders", sol::readonly_property([](const CollisionInfo& p_self)
            {
                return &p_self.m_colliders;
            }),
            "contacts", sol::readonly(&CollisionInfo::m_contacts)
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<CollisionInfo>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindContact(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Contact";

        sol::usertype resourceType = p_luaState.new_usertype<Contact>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "position", sol::readonly_property(&Contact::m_position),
            "impulse", sol::readonly_property(&Contact::m_impulse),
            "normal", sol::readonly_property(&Contact::m_normal),
            "distance", sol::readonly_property(&Contact::m_distance)
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Contact>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindTriggerInfo(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "TriggerInfo";

        sol::usertype resourceType = p_luaState.new_usertype<TriggerInfo>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "triggerCollider", sol::readonly_property(&TriggerInfo::m_triggerCollider),
            "otherCollider", sol::readonly_property(&TriggerInfo::m_otherCollider)
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<TriggerInfo>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindForceModes(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EForceMode";

        p_luaState.new_enum(typeName,
            "FORCE", EForceMode::FORCE,
            "IMPULSE", EForceMode::IMPULSE,
            "ACCELERATION", EForceMode::ACCELERATION,
            "VELOCITY_CHANGE", EForceMode::VELOCITY_CHANGE
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EForceMode>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindCollisionDetectionModes(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "ECollisionDetectionMode";

        p_luaState.new_enum(typeName,
            "DISCRETE", ECollisionDetectionMode::DISCRETE,
            "CONTINUOUS", ECollisionDetectionMode::CONTINUOUS,
            "CONTINUOUS_SPECULATIVE", ECollisionDetectionMode::CONTINUOUS_SPECULATIVE
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ECollisionDetectionMode>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindMaterial(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "PhysicsMaterial";

        sol::usertype resourceType = p_luaState.new_usertype<Material>(
            typeName,
            "staticFriction", &Material::m_staticFriction,
            "dynamicFriction", &Material::m_dynamicFriction,
            "restitution", &Material::m_restitution
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Material>(typeName);
        return (void)typeInfo;
    }
}
