#pragma once
#include "SurvivantScripting/LuaContext.h"

namespace sol
{
    class state;
}

namespace SvScripting::Bindings
{
    class LuaAudioBinder final
    {
    public:
        /**
         * \brief Creates or updates the audio bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void Bind(sol::state& p_luaState);

    private:
        /**
         * \brief Creates or updates the audio context bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindContext(sol::state& p_luaState);

        /**
         * \brief Creates or updates the audio handle bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindAudioHandle(sol::state& p_luaState);
    };
}
