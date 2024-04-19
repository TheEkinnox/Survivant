#pragma once
#include "SurvivantScripting/LuaContext.h"

#include <SurvivantCore/ECS/ComponentRegistry.h>
#include <SurvivantCore/ECS/ComponentTraits.h>
#include <SurvivantCore/Resources/ResourceRef.h>

#include <string>
#include <unordered_map>

#include <sol/sol.hpp>

namespace SvScripting
{
    class LuaScriptList
    {
    public:
        /**
         * \brief Creates an empty lua script list
         */
        LuaScriptList() = default;

        /**
         * \brief Creates a copy of the given lua script list
         * \param p_other The script list to copy
         */
        LuaScriptList(const LuaScriptList& p_other) = default;

        /**
         * \brief Creates a move copy of the given lua script list
         * \param p_other The script list to move
         */
        LuaScriptList(LuaScriptList&& p_other) noexcept = default;

        /**
         * \brief Destroys the lua script list
         */
        ~LuaScriptList();

        /**
         * \brief Assigns a copy of the given lua script list to this one
         * \param p_other The script list to copy
         * \return A reference to the modified script list
         */
        LuaScriptList& operator=(const LuaScriptList& p_other);

        /**
         * \brief Moves the given lua script list into this one
         * \param p_other The script list to move
         * \return A reference to the modified script list
         */
        LuaScriptList& operator=(LuaScriptList&& p_other) noexcept;

        /**
         * \brief Checks whether the given lua script is in the list or not
         * \param p_script The script to search for
         * \return True if the script was found. False otherwise
         */
        bool Contains(const std::string& p_script) const;

        /**
         * \brief Gets the given lua script's handle
         * \param p_script The script to get
         * \return A handle to the found script on success. An empty handle otherwise
         */
        LuaContext::ScriptHandle Get(const std::string& p_script) const;

        /**
         * \brief Adds the given lua script to the list
         * \param p_script The script to add
         * \param p_hint The added script's base table
         * \return A handle to the added script on success. An empty handle otherwise
         */
        LuaContext::ScriptHandle Add(std::string p_script, const sol::table& p_hint = sol::nil);

        /**
         * \brief Removes the given lua script from the list
         * \param p_script The script to remove
         */
        void Remove(const std::string& p_script);

        /**
         * \brief Clears the lua script list
         */
        void Clear();

    private:
        friend class SvCore::ECS::ComponentRegistry;
        friend struct SvCore::ECS::ComponentTraits;

        SvCore::ECS::EntityHandle                            m_owner;
        std::unordered_map<std::string, sol::table /*hint*/> m_scripts;
    };

    /**
     * \brief Deserializes the lua object from json
     * \param p_luaState The object's lua state
     * \param p_json The input json data
     * \param p_scene The object's target scene
     * \return The created object on success. A null optional otherwise
     */
    sol::optional<sol::object> LuaObjectFromJson(
        lua_State* p_luaState, const rapidjson::Value& p_json, SvCore::ECS::Scene* p_scene);

    /**
     * \brief Deserializes the lua object from binary
     * \param p_luaState The object's lua state
     * \param p_data The input memory buffer
     * \param p_length The input memory buffer's length
     * \param p_readBytes The output variable for the number of deserialized bytes
     * \param p_scene The object's target scene
     * \return The number of read bytes on success. 0 otherwise
     */
    sol::optional<sol::object> LuaObjectFromBinary(
        lua_State* p_luaState, const char* p_data, size_t p_length, size_t& p_readBytes, SvCore::ECS::Scene* p_scene);
}

namespace SvCore::ECS
{
    /**
     * \brief The action to perform when a lua script component is added
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnAdd(EntityHandle& p_entity, SvScripting::LuaScriptList& p_component);

    /**
     * \brief The action to perform when a lua script component is removed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnRemove(EntityHandle& p_entity, SvScripting::LuaScriptList& p_component);

    /**
     * \brief The action to perform before a lua script component is changed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnBeforeChange(EntityHandle& p_entity, SvScripting::LuaScriptList& p_component);

    /**
     * \brief The action to perform after a lua script component was changed
     * \param p_entity The component's owner
     * \param p_component The added component
     */
    template <>
    void ComponentTraits::OnChange(EntityHandle& p_entity, SvScripting::LuaScriptList& p_component);

    /**
     * \brief Serializes the given lua script component to json
     * \param p_component The serialized script component instance
     * \param p_writer The output json writer
     * \param p_toSerialized The entity to serialized entity map
     * \return True on success. False otherwise
     */
    template <>
    bool ComponentRegistry::ToJson(const SvScripting::LuaScriptList&           p_component,
                                   rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap& p_toSerialized);

    /**
     * \brief Deserializes the lua script component from json
     * \param p_out The output lua script component instance
     * \param p_json The input json data
     * \param p_scene The target scene
     * \return True on success. False otherwise
     */
    template <>
    bool ComponentRegistry::FromJson(SvScripting::LuaScriptList& p_out, const rapidjson::Value& p_json, Scene* p_scene);

    /**
     * \brief Serializes the given lua table to json
     * \param p_component The serialized lua table
     * \param p_writer The output json writer
     * \param p_toSerialized The entity to serialized entity map
     * \return True on success. False otherwise
     */
    template <>
    bool ComponentRegistry::ToJson(
        const sol::table& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap& p_toSerialized);

    /**
     * \brief Deserializes the lua table from json
     * \param p_out The output lua table
     * \param p_json The input json data
     * \param p_scene The target scene
     * \return True on success. False otherwise
     */
    template <>
    bool ComponentRegistry::FromJson(sol::table& p_out, const rapidjson::Value& p_json, Scene* p_scene);

    /**
     * \brief Serializes the given lua object to json
     * \param p_component The serialized lua object
     * \param p_writer The output json writer
     * \param p_toSerialized The entity to serialized entity map
     * \return True on success. False otherwise
     */
    template <>
    bool ComponentRegistry::ToJson(
        const sol::object& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap& p_toSerialized);
}
