#pragma once

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaResourceBinder final
    {
    public:
        /**
         * \brief Creates or updates the resource bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the resource reference bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindResourceRef(sol::state& p_luaState);

        /**
         * \brief Creates or updates the resource manager bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindResourceManager(sol::state& p_luaState);
    };
}
