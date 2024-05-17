#pragma once
#include "SurvivantScripting/LuaContext.h"

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaPhysicsBinder final
    {
    public:
        /**
         * \brief Creates or updates the physics bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the physics context bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindContext(sol::state& p_luaState);

        /**
         * \brief Creates or updates the rigid body bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindRigidBody(sol::state& p_luaState);

        /**
         * \brief Creates or updates the collision information bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindCollisionInfo(sol::state& p_luaState);

        /**
         * \brief Creates or updates the contact information bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindContact(sol::state& p_luaState);

        /**
         * \brief Creates or updates the trigger information bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindTriggerInfo(sol::state& p_luaState);

        /**
         * \brief Creates or updates the physics force mode bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindForceModes(sol::state& p_luaState);

        /**
         * \brief Creates or updates the physics collision detection mode bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindCollisionDetectionModes(sol::state& p_luaState);

        /**
         * \brief Creates or updates the physics material bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindMaterial(sol::state& p_luaState);

        /**
         * \brief Creates or updates the base collider bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindICollider(sol::state& p_luaState);

        /**
         * \brief Creates or updates the box collider bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindBoxCollider(sol::state& p_luaState);

        /**
         * \brief Creates or updates the sphere collider bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindSphereCollider(sol::state& p_luaState);

        /**
         * \brief Creates or updates the capsule collider bindings for the given lua state
         * \param p_luaState The target lua state
         */
        static void BindCapsuleCollider(sol::state& p_luaState);
    };
}
