#pragma once
#include <SurvivantCore/Resources/IResource.h>

namespace SvScripting
{
    class LuaScript final : public SvCore::Resources::IResource
    {
        REGISTERED_RESOURCE_BODY()

    public:
        using OrderT = int16_t;

        /**
         * \brief Creates an empty lua script
         */
        LuaScript();

        /**
         * \brief Creates a copy of the given lua script
         * \param p_other The lua script to copy
         */
        LuaScript(const LuaScript& p_other) = default;

        /**
         * \brief Creates a move copy of the given lua script
         * \param p_other The lua script to move
         */
        LuaScript(LuaScript&& p_other) noexcept = default;

        /**
         * \brief Destroys the lua script
         */
        ~LuaScript() override = default;

        /**
         * \brief Assigns a copy of the given lua script to this one
         * \param p_other The lua script to copy
         * \return A reference to the modified lua script
         */
        LuaScript& operator=(const LuaScript& p_other) = default;

        /**
         * \brief Moves the given lua script into this one
         * \param p_other The lua script to move
         * \return A reference to the modified lua script
         */
        LuaScript& operator=(LuaScript&& p_other) noexcept = default;

        /**
         * \brief Loads the lua script from the given path
         * \param p_fileName The script's path
         * \return True on success. False otherwise
         */
        bool Load(const std::string& p_fileName) override;

        /**
         * \brief Initializes the lua script
         * \return True on success. False otherwise
         */
        bool Init() override
        {
            return true;
        }

        /**
         * \brief Saves the meta data of the script at the given path
         * \param p_fileName The script's path
         * \return True on success. False otherwise
         */
        bool Save(const std::string& p_fileName) override;

        /**
         * \brief Gets the lua script's source code
         * \return The script's source
         */
        std::string_view GetSource() const;

        /**
         * \brief Gets the lua script's execution order
         * \return The script's execution order
         */
        OrderT GetExecutionOrder() const;

        /**
         * \brief Sets the lua script's execution order
         * \param p_executionOrder The script's new execution order
         */
        void SetExecutionOrder(OrderT p_executionOrder);

    private:
        std::string m_source;
        OrderT      m_executionOrder;

        /**
         * \brief Loads the lua script's meta data from the given path
         * \param p_path The lua script's meta path
         * \return True on success. False otherwise
         */
        bool LoadMeta(const std::string& p_path);

        /**
         * \brief Saves the lua script's meta data to the given path
         * \param p_path The lua script's meta path
         * \return True on success. False otherwise
         */
        bool SaveMeta(const std::string& p_path) const;
    };
}
