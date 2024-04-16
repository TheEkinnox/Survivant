//EditorEngine.cpp

#include "SurvivantEditor/Core/EditorEngine.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantEditor/Core/EditorWindow.h"

#include "SurvivantApp/Core/TempDefaultScene.h"


using namespace SvApp::Core;

namespace SvEditor::Core
{
	void EditorEngine::Update()
	{
		m_time.tick();

		if (m_editorWorld)
			m_editorWorld->Render();
		//if (m_PIEWorld)
		//	m_PIEWorld->Render();
	}

	std::weak_ptr<GameInstance> EditorEngine::CreatePIEGameInstance()
	{
		//if dont have PIE world context
		if (m_PIEWorld.expired())
		{
			ASSERT(false, "No PIE world on game instance creation");
			m_PIEWorld = CreatePIEWorldByDuplication(*m_editorWorld, m_editorSelectedScene);
		}

		m_gameInstance = std::make_shared<GameInstance>(m_PIEWorld);

		if (!InitializePlayInEditorGameInstance(*m_gameInstance))
			return std::weak_ptr<GameInstance>();

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

	std::shared_ptr<WorldContext> EditorEngine::CreatePIEWorldByDuplication(const WorldContext& p_context, std::shared_ptr<Scene> p_inScene)
	{
		auto pieWorld = Engine::CreateNewWorldContext(WorldContext::EWorldType::PIE);

		pieWorld->m_lightsSSBO = ToRemove::SetupLightSSBO(*p_inScene);

		pieWorld->m_owningGameInstance = p_context.m_owningGameInstance;
		pieWorld->m_viewport = p_context.m_viewport; //TODO : setup viewport when dupliucating world
		pieWorld->m_currentScene = p_inScene;
		pieWorld->InitCamera();
		pieWorld->SetSceneCamera();
		//pieWorld->m_persistentLevel = p_context.m_persistentLevel;

		return pieWorld;
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
		if (!(	PrepareSceneChange(p_worldContext, p_scene) &&
				CommitSceneChange(p_worldContext, p_scene)))
			return -1;

		//update selected scene, dont keep ref to current scene
		m_editorSelectedScene = p_worldContext.m_currentScene;

		return 1;
	}

	std::shared_ptr<Scene>* EditorEngine::GetCurrentScene()
	{
		return &m_currentScene;
	}

	std::shared_ptr<WorldContext> EditorEngine::CreateEditorDefaultWorld()
	{
		//TODO: better default loadded world

		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);

		world->m_owningGameInstance = nullptr;
		world->m_viewport = Vector2::zero();

		world->AddRenderPass(WorldContext::ERenderType::DEFAULT);
		world->AddRenderPass(WorldContext::ERenderType::ID);
		world->InitCamera();

		//world->m_persistentLevel = nullptr;

		return world;
	}

	bool EditorEngine::InitializePlayInEditorGameInstance(GameInstance& p_instance)
	{
		EditorEngine* EditorEngine = this;

		// Look for an existing pie world context, may have been created before
		auto& worldContext = EditorEngine->GetWorldContextRef(p_instance);
		ASSERT(!worldContext.expired(), "GameInstance has no world");

		worldContext.lock()->m_owningGameInstance = &p_instance;
		worldContext.lock()->m_lightsSSBO = ToRemove::SetupLightSSBO(*worldContext.lock()->m_currentScene);

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

		//create editor world world
		m_editorWorld = CreateEditorDefaultWorld();

		//create levels
		m_allLevels.emplace("Defaut_Scene", ToRemove::TestCreateDefaultScene());

		//load default level
		BrowseToDefaultScene(*m_editorWorld);
	}

	void EditorEngine::SetupUI(Core::EditorWindow* p_window, const std::array<std::function<void()>, 3>p_playPauseFrameCallbacks)
	{
		p_window->SetupUI(
			m_editorWorld,
			[this](const LibMath::Vector2I&)
			{
				ASSERT(m_PIEWorld.expired(), "Tried to create PIE world when already exists");

				auto PIEWorld =	CreatePIEWorldByDuplication(*m_editorWorld, m_editorSelectedScene);
				m_PIEWorld = std::weak_ptr<WorldContext>(PIEWorld);

				return PIEWorld;
			},
			p_playPauseFrameCallbacks);
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

		if (PrepareSceneChange(*m_PIEWorld.lock(), destination->second) &&
			CommitSceneChange(*m_PIEWorld.lock(), destination->second))
			return false;

		//update editorWorld level
		m_editorWorld->m_currentScene = destination->second;
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