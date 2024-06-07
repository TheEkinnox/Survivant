#pragma once

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaUtilityBinder final
    {
    public:
        /**
         * \brief Creates or updates the input bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the logger bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindLogger(sol::state& p_luaState);

        /**
         * \brief Creates or updates the timer bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindTimer(sol::state& p_luaState);

        /**
         * \brief Creates or updates the application bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindApplication(sol::state& p_luaState);
    };
}
