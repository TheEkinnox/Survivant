#pragma once
#include <SurvivantCore/Resources/IResource.h>
#include <SurvivantCore/Serialization/Serializer.h>

namespace SvPhysics
{
    class Material final : public SvCore::Resources::IResource
    {
        REGISTERED_RESOURCE_BODY()

    public:
        float m_staticFriction  = .6f;
        float m_dynamicFriction = .6f;
        float m_restitution     = 1.f;

        /**
         * \brief Creates a default physics material
         */
        Material() = default;

        /**
         * \brief Creates a copy of the given physics material
         * \param p_other The physics material to copy
         */
        Material(const Material& p_other) = default;

        /**
         * \brief Creates a move copy of the given physics material
         * \param p_other The physics material to move
         */
        Material(Material&& p_other) noexcept = default;

        /**
         * \brief Destroys the physics material
         */
        ~Material() override = default;

        /**
         * \brief Assigns a copy of the given physics material to this one
         * \param p_other The physics material to copy
         * \return A reference to the modified physics material
         */
        Material& operator=(const Material& p_other) = default;

        /**
         * \brief Moves the given physics material into this one
         * \param p_other The physics material to move
         * \return A reference to the modified physics material
         */
        Material& operator=(Material&& p_other) noexcept = default;

        /**
         * \brief Loads the physics material from the given file
         * \param p_fileName The path of the physics material to load
         * \return True if the physics material was successfully loaded. False otherwise.
         */
        bool Load(const std::string& p_fileName) override;

        bool Init() override
        {
            return true;
        }

        /**
         * \brief Saves the physics material at the given path
         * \param p_fileName The physics material's path
         * \return True on success. False otherwise
         */
        bool Save(const std::string& p_fileName) override;

        /**
         * \brief Serializes the physics material to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise.
         */
        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the material from json
         * \param p_json The input json data
         * \return True on success. False otherwise.
         */
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);
    };
}
