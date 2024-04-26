#pragma once

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaECSBinder final
    {
    public:
        /**
         * \brief Creates or updates the ECS bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the tag bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindTag(sol::state& p_luaState);

        /**
         * \brief Creates or updates the scene bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindScene(sol::state& p_luaState);

        /**
         * \brief Creates or updates the entity bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindEntity(sol::state& p_luaState);

        /**
         * \brief Creates or updates the component bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindComponent(sol::state& p_luaState);

        /**
         * \brief Creates or updates the script handle bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindScriptHandle(sol::state& p_luaState);

        /**
         * \brief Creates or updates the component search origin bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindComponentSearchOrigin(sol::state& p_luaState);
    };
}
