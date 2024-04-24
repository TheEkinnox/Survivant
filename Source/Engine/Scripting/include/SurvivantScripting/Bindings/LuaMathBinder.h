#pragma once
#include <SurvivantCore/Serialization/MathSerializers.h>

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaMathBinder final
    {
    public:
        /**
         * \brief Creates or updates the math bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the angle bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindAngles(sol::state& p_luaState);

        /**
         * \brief Creates or updates the rotation order bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindRotationOrder(sol::state& p_luaState);

        /**
         * \brief Creates or updates the matrix bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindMatrix(sol::state& p_luaState);

        /**
         * \brief Creates or updates the 2D vector bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindVector2(sol::state& p_luaState);

        /**
         * \brief Creates or updates the 3D vector bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindVector3(sol::state& p_luaState);

        /**
         * \brief Creates or updates the 4D vector bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindVector4(sol::state& p_luaState);

        /**
         * \brief Creates or updates the quaternion bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindQuaternion(sol::state& p_luaState);

        /**
         * \brief Creates or updates the transform bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindTransform(sol::state& p_luaState);

        /**
         * \brief Creates or updates the global math bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindGlobals(const sol::state& p_luaState);
    };
}
