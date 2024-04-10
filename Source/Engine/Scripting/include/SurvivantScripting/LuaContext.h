#pragma once
#include "SurvivantScripting/LuaScriptComponent.h"

#include <SurvivantCore/ECS/EntityHandle.h>

#include <sol/sol.hpp>

namespace SvScripting
{
    class LuaContext
    {
    public:
        /**
         * \brief Creates a lua context
         */
        LuaContext();

        /**
         * \brief Disable lua context copying
         */
        LuaContext(const LuaContext&) = delete;

        /**
         * \brief Creates a move copy of the given lua context
         * \param p_other The lua context to move
         */
        LuaContext(LuaContext&& p_other) noexcept = default;

        /**
         * \brief Destroys the lua context
         */
        ~LuaContext();

        /**
         * \brief Disable lua context copying
         */
        LuaContext& operator=(const LuaContext&) = delete;

        /**
         * \brief Moves the given lua context into this one
         * \param p_other The lua context to move
         * \return A reference to the modified lua context
         */
        LuaContext& operator=(LuaContext&& p_other) noexcept = default;

        /**
         * \brief Gets the lua context instance
         * \return The lua context instance
         */
        static LuaContext& GetInstance();

        /**
         * \brief Initializes the lua context
         */
        void Init();

        /**
         * \brief Resets the lua context
         */
        void Reset();

        /**
         * \brief Checks whether the context is in a valid state or not
         * \return True if the context is in a valid state. False otherwise
         */
        bool IsValid() const;

        /**
         * \brief Registers the given script to the lua context
         * \param p_entity The registered script's owner
         * \param p_script The script to register to the context
         * \return True on success. False otherwise
         */
        bool RegisterScript(const SvCore::ECS::EntityHandle& p_entity, LuaScriptComponent& p_script);

        /**
         * \brief Adds the given script to the lua context
         * \param p_entity The added script's owner
         * \param p_script The script to add to the context
         * \return True on success. False otherwise
         */
        bool AddScript(const SvCore::ECS::EntityHandle& p_entity, LuaScriptComponent& p_script);

        /**
         * \brief Adds the given script to the lua context
         * \param p_entity The removed script's owner
         */
        void RemoveScript(SvCore::ECS::EntityHandle& p_entity);

        /**
         * \brief Calls the function with the given name and parameters on the given script
         * \tparam Args The function parameter types
         * \param p_component The script on which the function should be called
         * \param p_name The function's name
         * \param p_args The function's parameters
         * \return True if the function could be called. False otherwise
         */
        template <typename... Args>
        bool TryCall(LuaScriptComponent& p_component, const std::string& p_name, Args&&... p_args);

        /**
         * \brief Initializes and starts all the registered scripts
         */
        void Start();

        /**
         * \brief Updates all the registered scripts every frame
         * \param p_deltaTime The elapsed time since the last update
         */
        void Update(float p_deltaTime);

        /**
         * \brief Stops all the registered scripts
         */
        void Stop();

    private:
        using ScriptHandle = std::pair<LuaScript::OrderT, SvCore::ECS::EntityHandle>;

        std::unique_ptr<sol::state> m_state;
        std::vector<ScriptHandle>   m_scripts;

        bool m_isValid, m_hasStarted;

        /**
         * \brief Loads a module from the given lua state
         * \param p_state The calling lua state
         * \return The number of elements left in the stack
         */
        static int LoadModule(lua_State* p_state);
    };
}

#include "SurvivantScripting/LuaContext.inl"
