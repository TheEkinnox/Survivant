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
         * \brief Creates or updates the model component bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindModelComponent(sol::state& p_luaState);
    };
}
