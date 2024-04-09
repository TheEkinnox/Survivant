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
		void Render();

		std::shared_ptr<SvCore::ECS::Scene>* GetCurrentLevelPtr()
		{
			return &m_currentScene;
		}

		EWorldType	m_worldType = EWorldType::NONE;
		int			m_viewport = 0;
		int			m_camera = 0;

		GameInstance* m_owningGameInstance = nullptr;
		std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer> m_lightsSSBO = nullptr;
		SvRendering::RHI::IFrameBuffer*							m_frameBuffer = nullptr;
		//TODO: deal with persistentLevel
		//std::shared_ptr<Scene>				m_persistentLevel = nullptr;

	private:
		std::shared_ptr<SvCore::ECS::Scene>		m_currentScene = nullptr;
	};

	
}