//BuildConfig.h
#pragma once

#include <SurvivantCore/Resources/IResource.h>
#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/ECS/Scene.h>

namespace SvApp::Core
{
	class BuildConfig : public SvCore::Resources::IResource
	{
        REGISTERED_RESOURCE_BODY()

	public:
        /**
         * \brief Creates a default BuildConfig
         */
        BuildConfig() = default;

        /**
         * \brief Creates a BuildConfig with the given scene
         * \param p_scene The BuildConfig's start scene
         */
        explicit BuildConfig(const SvCore::Resources::ResourceRef<SvCore::ECS::Scene>& p_scene);

        /**
         * \brief Creates a copy of the given BuildConfig
         * \param p_other The BuildConfig to copy
         */
        BuildConfig(const BuildConfig& p_other) = default;

        /**
         * \brief Creates a move copy of the given material
         * \param p_other The material to move
         */
        BuildConfig(BuildConfig&& p_other) noexcept = default;

        /**
         * \brief Destroys the BuildConfig
         */
        ~BuildConfig() = default;

        /**
         * \brief Assigns a copy of the given BuildConfig to this one
         * \param p_other The BuildConfig to copy
         * \return A reference to the modified BuildConfig
         */
        BuildConfig& operator=(const BuildConfig& p_other) = default;

        /**
         * \brief Moves the given BuildConfig into this one
         * \param p_other The BuildConfig to move
         * \return A reference to the modified BuildConfig
         */
        BuildConfig& operator=(BuildConfig&& p_other) noexcept = default;

        // Inherited via IResource
        bool Init() override;
        bool Load(const std::string& p_fileName) override;
        bool Save(const std::string& p_fileName) override;

        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);

        SvCore::Resources::ResourceRef<SvCore::ECS::Scene> m_scene;
    };
}


