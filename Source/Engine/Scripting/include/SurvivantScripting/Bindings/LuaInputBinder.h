#pragma once
#include <Vector/Vector2.h>

namespace SvApp
{
    class InputManager;
}

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
         * \brief Creates or updates the cursor mode bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindCursorModes(sol::state& p_luaState);

        /**
         * \brief Creates or updates the input functions bindings in the given lua state
         * \param p_luaState The target lua state
         */
        static void BindFunctions(sol::state& p_luaState);

        /**
         * \brief Gets the current cursor position
         * \param p_self The target input manager
         * \return The current cursor position
         */
        static LibMath::Vector2 GetMousePos(const SvApp::InputManager& p_self);

        /**
         * \brief Sets the cursor position
         * \param p_self The target input manager
         * \param p_pos The new cursor position
         */
        static void SetMousePos(SvApp::InputManager& p_self, const LibMath::Vector2& p_pos);

        /**
         * \brief Gets the current cursor position
         * \param p_self The target input manager
         * \return The current cursor position
         */
        static LibMath::Vector2 GetMouseDelta(const SvApp::InputManager& p_self);
    };
}
