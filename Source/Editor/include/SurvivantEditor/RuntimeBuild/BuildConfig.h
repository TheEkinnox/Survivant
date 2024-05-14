//BuildConfig.h
#pragma once

#include <SurvivantCore/Resources/IResource.h>
#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/ECS/Scene.h>

namespace SvEditor::RuntimeBuild
{
	class BuildConfig
	{
	public:
        using Scenes = std::vector<SvCore::Resources::ResourceRef<SvCore::ECS::Scene>>;

        /**
         * \brief Creates a default BuildConfig
         */
        BuildConfig() = default;

        /**
         * \brief Creates a BuildConfig with the given scenes
         * \param p_scenes The BuildConfig's scenes
         */
        explicit BuildConfig(const Scenes& p_scenes);

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

	private:
        Scenes      m_scenes;
	};
}


