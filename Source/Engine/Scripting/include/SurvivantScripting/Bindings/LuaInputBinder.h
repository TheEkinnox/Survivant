#pragma once

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaInputBinder final
    {
    public:
        /**
         * \brief Creates or updates the input bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the key bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindKeys(sol::state& p_luaState);

        /**
         * \brief Creates or updates the mouse button bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindMouseButtons(sol::state& p_luaState);

        /**
         * \brief Creates or updates the input modifier bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindInputModifiers(sol::state& p_luaState);

        /**
         * \brief Creates or updates the input functions bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindFunctions(sol::state& p_luaState);
    };
}
