//EditorEngine.cpp
#pragma once

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/EditorEngine.h"
#include "SurvivantEditor/App/GameInstance.h"

namespace SvEditor::App
{
	void EditorEngine::Update()
	{
		m_time.tick();

		//if (m_gameInstance != nullptr)
		//{
		//	m_gameInstance->Update();
		//}
	}

	std::weak_ptr<GameInstance> EditorEngine::CreatePIEGameInstance()
	{
		m_gameInstance = std::make_shared<GameInstance>();

		bool initResukt = InitializePlayInEditorGameInstance(*m_gameInstance);
		if (!initResukt)
			return std::weak_ptr<GameInstance>();
		
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

		return std::weak_ptr<GameInstance>(m_gameInstance);
	}

	void EditorEngine::DestroyGameInstance()
	{
		m_gameInstance.reset();
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

		pieWorld->m_owningGameInstance = p_context.m_owningGameInstance;
		pieWorld->m_viewport = p_context.m_viewport;
		//pieWorld->m_persistentLevel = p_context.m_persistentLevel;
		//pieWorld->m_levels = p_context.m_levels;

		return pieWorld;
	}

	std::shared_ptr<Engine::WorldContext> EditorEngine::CreateEditorDefaultWorld()
	{
		//TODO: better default loadded world

		auto world = std::make_shared<Engine::WorldContext>();
		world->m_worldType = EWorldType::EDITOR;

		world->m_owningGameInstance = nullptr;
		world->m_viewport = 0;
		//world->m_persistentLevel = nullptr;

		return world;
	}

	bool EditorEngine::InitializePlayInEditorGameInstance(GameInstance& p_instance)
	{
		EditorEngine* EditorEngine = this;

		// Look for an existing pie world context, may have been created before
		auto& worldContext = EditorEngine->GetWorldContextRef(p_instance);

		if (worldContext.get() == nullptr)
		{
			// If not, create a new one
			worldContext = EditorEngine->CreateNewWorldContext(EWorldType::PIE);
		}

		worldContext->m_owningGameInstance = &p_instance;

		//init
		p_instance.Init();


		////TODO: do we install a garbage collector?
		// add world to garbage collector

		//TODO : find and set main camera

		//start level
		StartLevel(*worldContext);

		return true;
	}

	void EditorEngine::Init()
	{
		g_engine = this;

		//TODO: load all ressources here

		//create world
		m_PIEWorld = CreateEditorDefaultWorld();

		//create levels
		m_allLevels.push_back(std::make_shared<Level>());
		m_allLevels[0]->InitLevel("ADD_DEFAULT_PATH_HERE");
		m_allLevels[0]->m_name = "test_level";

		//load default level
		BrowseToDefaultLevel(*m_PIEWorld);
	}

	bool EditorEngine::StartLevel(WorldContext& p_worldContext)
	{
		p_worldContext.m_currentLevel->BeginPlay();

		return true;
	}

	void EditorEngine::RedrawViewports()
	{
		//TODO: redraw viewports here
	}
	float EditorEngine::GetDeltaTime()
	{
		return m_time.getDeltaTime();
	}
}