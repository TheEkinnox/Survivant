#pragma once

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaRenderingBinder final
    {
    public:
        /**
         * \brief Creates or updates the rendering bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the color bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindColor(sol::state& p_luaState);

        /**
         * \brief Creates or updates the layer bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindLayers(sol::state& p_luaState);

        /**
         * \brief Creates or updates the projection type bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindProjectionTypes(sol::state& p_luaState);

        /**
         * \brief Creates or updates the clear flag bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindClearFlags(sol::state& p_luaState);

        /**
         * \brief Creates or updates the model component bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindModelComponent(sol::state& p_luaState);

        /**
         * \brief Creates or updates the camera component bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindCameraComponent(sol::state& p_luaState);
    };
}
