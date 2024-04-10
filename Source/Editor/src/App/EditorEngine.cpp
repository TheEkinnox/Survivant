//EditorEngine.cpp

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/EditorEngine.h"
#include "SurvivantEditor/App/GameInstance.h"

#include "SurvivantEditor/App/TempDefaultScene.h"

namespace SvEditor::App
{
	void EditorEngine::Update()
	{
		m_time.tick();

		if (m_gameInstance.get() != nullptr)
		{
			WorldContext::IdRender(*GetWorldContextRef(*m_gameInstance));		
		}
	}

	std::weak_ptr<GameInstance> EditorEngine::CreatePIEGameInstance()
	{
		//if dont have PIE world context
		if (m_PIEWorld == nullptr)
		{
			m_PIEWorld = CreatePIEWorldByDuplication(*m_editorWorld, m_editorSelectedScene);
		}

		m_gameInstance = std::make_shared<GameInstance>(m_PIEWorld);

		if (!InitializePlayInEditorGameInstance(*m_gameInstance))
			return std::weak_ptr<GameInstance>();
		
		//m_PIEWorld->m_owningGameInstance = m_gameInstance.get();
		//m_PIEWorld->m_lightsSSBO = ToRemove::SetupLightSSBO(**m_PIEWorld->GetCurrentLevelPtr());

		return std::weak_ptr<GameInstance>(m_gameInstance);
	}

	void EditorEngine::DestroyGameInstance()
	{
		m_gameInstance.reset();
		m_gameInstance = nullptr;

		//while playing, loaded new scene, so go back to selected
		if (m_currentScene != m_editorSelectedScene)
			BrowseToScene(*m_editorWorld, m_editorSelectedScene);
	}

	int EditorEngine::BrowseToDefaultScene(WorldContext& p_worldContext)
	{
		ASSERT(!m_allLevels.empty(), "No levels to browse to");
		auto val = BrowseToScene(p_worldContext, m_allLevels.begin()->second);

		return val;
	}

	int EditorEngine::BrowseToScene(WorldContext& p_worldContext, std::shared_ptr<Scene> p_scene)
	{
		ASSERT(!m_allLevels.empty(), "No levels to browse to");

		//not a valid scene
		if (p_scene.get() == nullptr)
			return -1;

		//alredy in scene
		if (m_editorSelectedScene.get() != nullptr && p_scene == m_editorSelectedScene)
			return 0;
		
		//update current scene
		if (!(	PrepareLevelChange(p_worldContext, p_scene) &&
				CommitLevelChange(p_worldContext, p_scene)))
			return -1;

		//update selected scene, dont keep ref to current scene
		m_editorSelectedScene = *p_worldContext.GetCurrentLevelPtr();

		return 1;
	}

	std::shared_ptr<Scene>* EditorEngine::GetCurrentScene()
	{
		return &m_currentScene;
	}

	std::shared_ptr<WorldContext> EditorEngine::CreatePIEWorldByDuplication(WorldContext& p_context, std::shared_ptr<Scene> p_inLevel)
	{
		auto pieWorld = Engine::CreateNewWorldContext(WorldContext::EWorldType::PIE);
		*pieWorld->GetCurrentLevelPtr() = p_inLevel;

		pieWorld->m_lightsSSBO = ToRemove::SetupLightSSBO(**pieWorld->GetCurrentLevelPtr());

		pieWorld->m_owningGameInstance = p_context.m_owningGameInstance;
		pieWorld->m_viewport = p_context.m_viewport; //TODO : setup viewport when dupliucating world
		*pieWorld->GetCurrentLevelPtr() = *p_context.GetCurrentLevelPtr();
		//pieWorld->m_persistentLevel = p_context.m_persistentLevel;

		return pieWorld;
	}

	std::shared_ptr<WorldContext> EditorEngine::CreateEditorDefaultWorld()
	{
		//TODO: better default loadded world

		auto world = std::make_shared<WorldContext>();
		world->m_worldType = WorldContext::EWorldType::EDITOR;

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
		ASSERT(worldContext.get() != nullptr, "GameInstance has no world");

		worldContext->m_owningGameInstance = &p_instance;
		worldContext->m_lightsSSBO = ToRemove::SetupLightSSBO(**worldContext->GetCurrentLevelPtr());

		//init
		p_instance.Init();

		////TODO: do we install a garbage collector?
		// add world to garbage collector

		//TODO : find and set main camera

		//dont start here
		//worldContext->BeginPlay();

		return true;
	}

	void EditorEngine::Init()
	{
		g_engine = this;

		//TODO: load all ressources here

		//create world
		m_editorWorld = CreateEditorDefaultWorld();

		//create levels
		m_allLevels.emplace("Defaut_Scene", ToRemove::TestCreateDefaultScene());
		//m_allLevels["test_level"]->Load("ADD_DEFAULT_PATH_HERE");

		//load default level
		BrowseToDefaultScene(*m_editorWorld);
	}

	//bool EditorEngine::StartScene(WorldContext& p_worldContext)
	//{
	//	p_worldContext.BeginPlay();

	//	return true;
	//}

	bool EditorEngine::ChangeScene(const std::string& p_sceneName)
	{
		ASSERT(!m_allLevels.empty(), "No levels to browse to");
		//ASSERT(p_worldContext.m_currentScene != nullptr); can have no current if first browse

		auto destination = m_allLevels.find(p_sceneName);

		//not a valid scene name
		if (destination == m_allLevels.end())
			return false;

		if (PrepareLevelChange(*m_PIEWorld, destination->second) &&
			CommitLevelChange(*m_PIEWorld, destination->second))
			return false;

		//update editorWorld level
		*m_editorWorld->GetCurrentLevelPtr() = destination->second;
		//dont update selected editor scene

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