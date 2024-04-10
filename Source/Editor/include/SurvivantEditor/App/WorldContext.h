//Engine.h
#pragma once

//#include "SurvivantEditor/App/GameInstance.h"

#include "SurvivantCore/ECS/Scene.h"

#include "SurvivantRendering/RHI/IShaderStorageBuffer.h"
#include "SurvivantRendering/RHI/IFrameBuffer.h"

#include <string>
#include <memory>
#include <vector>

namespace SvEditor::App
{
	//foward declaration
	class GameInstance;

	struct WorldContext
	{
		enum class EWorldType
		{
			NONE,
			PIE,
			EDITOR,
			GAME
		};

		void BeginPlay();
		void Update();

		std::shared_ptr<SvCore::ECS::Scene>* GetCurrentLevelPtr()
		{
			return &m_currentScene;
		}

		bool		m_isDisplayed = true;
		EWorldType	m_worldType = EWorldType::NONE;
		int			m_viewport = 0;
		int			m_camera = 0;

		GameInstance* m_owningGameInstance = nullptr;
		std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer> m_lightsSSBO = nullptr;
		SvRendering::RHI::IFrameBuffer*							m_frameBuffer = nullptr;

		/*std::unique_ptr<SvRendering::RHI::IFrameBuffer>			m_frameBuffer = nullptr;
		LibMath::Vector2										m_viewport;*/


		//TODO: deal with persistentLevel
		//std::shared_ptr<Scene>				m_persistentLevel = nullptr;

		//move to framebuffer
		static void DefaultRender(const WorldContext& p_world);
		static void IdRender(const WorldContext& p_world);

	private:
		std::shared_ptr<SvCore::ECS::Scene>		m_currentScene = nullptr;

	};

	
}