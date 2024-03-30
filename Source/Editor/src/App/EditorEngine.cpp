//EditorEngine.cpp
#pragma once

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/EditorEngine.h"
#include "SurvivantEditor/App/GameInstance.h"

namespace SvEditor::App
{
	void EditorEngine::Update()
	{
		if (m_gameInstance != nullptr)
		{
			m_gameInstance->Update();
		}
	}

	GameInstance* EditorEngine::CreatePIEGameInstance()
	{
		m_gameInstance = std::make_unique<GameInstance>();

		bool initResukt = InitializePlayInEditorGameInstance(*m_gameInstance);
		if (!initResukt)
			return nullptr;
		

		//if dont have PIE world context
		if (m_PIEWorld == nullptr)
		{
			m_PIEWorld = CreatePIEWorldByDuplication(*m_editorWorld, m_editorLevel);
		}
		else
		{
			//maybe?
			//m_PIEWorld->m_levels = m_editorWorld->m_levels;
			//m_PIEWorld->m_owningGameInstance = m_editorWorld->m_owningGameInstance;
			//m_PIEWorld->m_persistentLevel = m_editorWorld->m_persistentLevel;
			//m_PIEWorld->m_viewport = m_editorWorld->m_viewport;

			m_PIEWorld->m_currentLevel = m_editorLevel;
		}

		return m_gameInstance.get();
	}

	void EditorEngine::DestroyGameInstance()
	{
		m_gameInstance.release();
		m_gameInstance = nullptr;

	}

	std::shared_ptr<Level>* EditorEngine::GetCurrentLevel()
	{
		return &m_currentLevel;
	}

	std::shared_ptr<Engine::WorldContext> EditorEngine::GetPIEWorldContext()
	{
		return m_currentLevel->m_gameInstance->m_worldContext;
	}

	std::shared_ptr<Engine::WorldContext> EditorEngine::CreatePIEWorldByDuplication(WorldContext& p_context, std::shared_ptr<Level> p_inLevel)
	{
		auto pieWorld = Engine::CreateNewWorldContext(EWorldType::PIE);
		pieWorld->m_currentLevel = p_inLevel;

		pieWorld->m_levels = p_context.m_levels;
		pieWorld->m_owningGameInstance = p_context.m_owningGameInstance;
		pieWorld->m_persistentLevel = p_context.m_persistentLevel;
		pieWorld->m_viewport = p_context.m_viewport;

		return pieWorld;
	}

	bool EditorEngine::InitializePlayInEditorGameInstance(GameInstance& p_instance)
	{
		EditorEngine* EditorEngine = this;

		// Look for an existing pie world context, may have been created before
		auto& worldContext = EditorEngine->GetWorldContextRef(p_instance);

		if (worldContext != nullptr)
		{
			// If not, create a new one
			worldContext = EditorEngine->CreateNewWorldContext(EWorldType::PIE);
		}

		worldContext->m_owningGameInstance = &p_instance;

		//init
		p_instance.Init();

		std::shared_ptr<Level>& newLevel = this->m_currentLevel;

		newLevel->m_gameInstance = &p_instance;
		worldContext->m_currentLevel = newLevel;


		newLevel->InitLevel();

		////TODO: do we install a garbage collector?
		// add world to garbage collector

		//TODO : find and set main camera

		return true;
	}

	void EditorEngine::Init()
	{
		//TODO: load all ressources here
	}

	bool EditorEngine::LoadNewLevel(WorldContext& p_worldContext, std::shared_ptr<Level> p_level)
	{
		auto& allLevels = p_worldContext.m_levels;
		if (!CHECK(std::find(allLevels.begin(), allLevels.end(), p_level) != allLevels.end()))
			return false;

		p_level->InitLevel();

		if (!CHECK(p_level->m_bIsInitialized))
			return false;
		
		auto& m_levelToDeInit = p_worldContext.m_currentLevel;
		p_worldContext.m_currentLevel = p_level;

		if (m_levelToDeInit != nullptr)
			m_levelToDeInit->DeInitLevel();

		return true;
	}

	void EditorEngine::RedrawViewports()
	{
		//TODO: redraw viewports here
	}
}