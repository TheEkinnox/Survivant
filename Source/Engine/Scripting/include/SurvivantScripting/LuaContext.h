#pragma once
#include "SurvivantScripting/LuaScriptHandle.h"

#include <SurvivantCore/Resources/ResourceRef.h>

#include <cstdint>

#include <sol/sol.hpp>

namespace SvScripting
{
    enum class ELuaCallResult : uint8_t
    {
        INVALID_STATE,
        NOT_FOUND,
        SUCCESS,
        FAILURE
    };

    class LuaContext
    {
    public:
        using Binder = void(*)(sol::state& p_state);

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
         * \brief Gets the current lua context instance
         * \return The current lua context instance
         */
        static LuaContext& GetInstance();

        /**
         * \brief Initializes the lua context
         */
        void Init();

        /**
         * \brief Removes all loaded scripts from the lua context
         */
        void Clear();

        /**
         * \brief Resets the lua context
         */
        void Reset();

        /**
         * \brief Reloads the lua context
         */
        void Reload();

        /**
         * \brief Checks whether the context is in a valid state or not
         * \return True if the context is in a valid state. False otherwise
         */
        bool IsValid() const;

        /**
         * \brief Registers the given script to the lua context
         * \param p_handle The script to register to the context
         * \return True on success. False otherwise
         */
        bool RegisterScript(LuaScriptHandle& p_handle);

        /**
         * \brief Adds the given script to the lua context
         * \param p_script The script to add to the context
         * \param p_owner The added script's owner
         * \param p_hint The added script's base table
         * \return A handle to the added script on success. An empty handle otherwise
         */
        LuaScriptHandle AddScript(const std::string& p_script, const SvCore::ECS::EntityHandle& p_owner, const sol::table& p_hint);

        /**
         * \brief Gets a handle to the given script owned by the given entity
         * \param p_script The script to get a handle for
         * \param p_owner The script's owner
         * \return A handle to the found script. An empty handle if the script wasn't found
         */
        LuaScriptHandle GetScript(const std::string& p_script, const SvCore::ECS::EntityHandle& p_owner) const;

        /**
         * \brief Removes the given script from the lua context
         * \param p_script The script to remove
         * \param p_owner The removed script's owner
         */
        void RemoveScript(const std::string& p_script, SvCore::ECS::EntityHandle& p_owner);

        /**
         * \brief Calls the function with the given name and parameters on the given lua object
         * \tparam Args The function parameter types
         * \param p_table The lua object on which the function should be called
         * \param p_name The function's name
         * \param p_args The function's parameters
         * \return The lua call result
         */
        template <typename... Args>
        ELuaCallResult TryCall(sol::table& p_table, const std::string& p_name, Args&&... p_args);

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

        /**
         * \brief Gets the current native lua state
         * \return The current native lua state
         */
        lua_State* GetLuaState() const;

        /**
         * \brief Gets the given module's name
         * \param p_module The target module
         * \return The module's name
         */
        static const std::string& GetModuleName(std::string p_module);

        /**
         * \brief Gets the given module's path
         * \param p_module The target module
         * \param p_fromGetName Whether the function was called from GetModuleName
         * \return The module's path
         */
        static std::string GetModulePath(std::string p_module, bool p_fromGetName = false);

        /**
         * \brief Registers the given user type binder function
         * \param p_binder The binder function to register
         */
        static void SetUserTypeBinders(Binder p_binder = &LuaContext::DefaultUserTypeBindings);

    private:
        using ListenerId = SvCore::Events::Event<>::ListenerId;

        static constexpr const char* EXTENSIONS[] = { ".lua", ".lc" };

        inline static std::unordered_map<std::string, std::string> s_moduleNames;
        inline static std::unordered_map<std::string, std::string> s_modulePaths;
        inline static Binder                                       s_userTypeBinders;

        std::unique_ptr<sol::state>  m_state;
        std::vector<LuaScriptHandle> m_scripts;

        ListenerId m_collisionListenerId;
        ListenerId m_triggerListenerId;

        bool m_isValid, m_hasStarted;

        /**
         * \brief Loads a module from the given lua state
         * \param p_luaState The calling lua state
         * \return The number of elements left in the stack
         */
        static int LoadModule(lua_State* p_luaState);

        /**
         * \brief Binds the known default user types to the given lua state
         * \param p_luaState The lua state to bind to
         */
        static void DefaultUserTypeBindings(sol::state& p_luaState);

        /**
         * \brief Subscribes the lua context to the physics context's events
         */
        void LinkPhysicsEvents();

        /**
         * \brief Unsubscribes the lua context from the physics context's events
         */
        void UnlinkPhysicsEvents();
    };
}

#include "SurvivantScripting/LuaContext.inl"
