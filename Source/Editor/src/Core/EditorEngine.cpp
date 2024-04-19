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

		m_editorWorld->m_renderingContext->UpdateCameraInput();

		//if (m_editorWorld)
		//	m_editorWorld->Render();

		//if (!m_PIEWorld.expired())
		//	m_PIEWorld.lock()->Render();
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

		m_editorWorld->SetInputs();
		m_editorWorld->LoadCurrentScene();

		m_PIEWorld.lock()->Render();
	}

	std::shared_ptr<WorldContext> EditorEngine::CreatePIEWorldByDuplication(const WorldContext& p_context, std::shared_ptr<Scene> p_inScene)
	{
		auto pieWorld = Engine::CreateNewWorldContext(WorldContext::EWorldType::PIE);


		pieWorld->m_owningGameInstance = p_context.m_owningGameInstance;
		pieWorld->m_viewport = p_context.m_viewport; //TODO : setup viewport when dupliucating world
		pieWorld->CurrentScene() = p_inScene;
		pieWorld->SetSceneCamera(pieWorld->GetDefaultSceneCamera());
		pieWorld->m_inputs = ToRemove::SetupGameInputs();
		pieWorld->m_lightsSSBO = ToRemove::SetupLightSSBO(*p_inScene);

		pieWorld->Render();
		//pieWorld->m_persistentLevel = p_context.m_persistentLevel;

		return pieWorld;
	}

	std::tuple<int, int> AddInputTranslate(char i)
	{
		return { i, 10 };
	}

	std::tuple<int, int> AddMouseTranslate(float i, float j)
	{
		return { (int)i, (int)j };
	}

	std::tuple<> SpaceTranslate(char /*c*/)
	{
		return { };
	}

	std::shared_ptr<EditorEngine::Inputs> EditorEngine::CreateEditorInputs()
	{
		using namespace SvCore;
		using namespace SvApp;
		using namespace Events;
		using AddEvent = Events::Event<int, int>;
		class ToggleEvent : public Events::Event<> {};

		InputManager& im = InputManager::GetInstance();
		std::shared_ptr<EditorEngine::Inputs> input = std::make_shared<EditorEngine::Inputs>();

		auto& k = input->m_keyCallbacks;
		auto& m = input->m_mouseKeyCallbacks;

		EventManager& em = EventManager::GetInstance();
		AddEvent::EventDelegate printAdd = [](int i, int j) { std::cout << "Add = " << i + j << std::endl; };
		//ToggleEvent::EventDelegate toggle = std::bind(&SvApp::Window::ToggleFullScreenMode, &window);
		em.AddListenner<AddEvent>(printAdd);
		//em.AddListenner<ToggleEvent>(toggle);

		//cant put MODS bsc of imgui
		InputManager::KeyboardKeyType   a(EKey::A, EKeyState::RELEASED, EInputModifier());
		InputManager::KeyboardKeyType   b(EKey::B, EKeyState::PRESSED, EInputModifier());
		InputManager::MouseKeyType      mouse(EMouseButton::MOUSE_1, EMouseButtonState::PRESSED, EInputModifier());
		InputManager::KeyboardKeyType   space(EKey::SPACE, EKeyState::PRESSED, EInputModifier());

		auto test = [](char p_c) { return std::tuple<int, int>{ p_c, 10 }; };
		//k.emplace(a, im.CreateInputEventBinding<AddEvent>(a, &AddInputTranslate));
		//k.emplace(b, im.CreateInputEventBinding<AddEvent>(a, &AddInputTranslate));
		k.emplace(space, im.CreateInputEventBinding<AddEvent>(space, &AddInputTranslate));
		k.emplace(InputManager::KeyboardKeyType{ EKey::ESCAPE, EKeyState::PRESSED, EInputModifier() },
			[this](char) { m_isRunning = false; });

		m.emplace(mouse, im.CreateInputEventBinding<AddEvent>(mouse, &AddMouseTranslate));

		//move camera in scene
		k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				m_editorWorld->m_renderingContext->CameraMoveInput().m_y += 1;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				m_editorWorld->m_renderingContext->CameraMoveInput().m_y -= 1;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});


		return input;
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
		m_editorSelectedScene = p_worldContext.CurrentScene();

		return 1;
	}

	std::shared_ptr<Scene>* EditorEngine::GetCurrentScene()
	{
		return &m_currentScene;
	}

	std::shared_ptr<WorldContext> EditorEngine::CreateEditorDefaultWorld(std::shared_ptr<Scene> p_inScene)
	{
		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);
		world->m_owningGameInstance = nullptr;

		world->m_lightsSSBO = ToRemove::SetupLightSSBO(*p_inScene);
		world->m_viewport = { 800, 600 };
		world->CurrentScene() = p_inScene;
		auto cam = Camera(perspectiveProjection(120_deg, 4.f / 3.f, .01f, 14.f));
		cam.SetClearColor(Color::black);
		world->SetOwningCamera(cam, Transform({ 0.f, 1.8f, 2.f }, Quaternion::identity(), Vector3::one()));
		world->m_inputs = CreateEditorInputs();

		//load and render
		//world->LoadCurrentScene();
		world->Render();

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
		worldContext.lock()->m_lightsSSBO = ToRemove::SetupLightSSBO(*worldContext.lock()->CurrentScene());

		worldContext.lock()->SetInputs();

		//init
		p_instance.Init();

		//TODO : find and set main camera

		//dont start here
		//worldContext->BeginPlay();

		return true;
	}

	void EditorEngine::Init()
	{
		g_engine = this;

		//TODO: load all ressources here

		//create scenes
		auto defaultScene = m_allLevels.emplace("Defaut_Scene", ToRemove::TestCreateDefaultScene());
		m_editorSelectedScene = defaultScene.first->second;

		//create editor world world
		m_editorWorld = CreateEditorDefaultWorld(defaultScene.first->second);
		m_editorWorld->SetInputs();
	}

	void EditorEngine::RenderWorlds()
	{
		if (m_editorWorld->m_isVisalbe)
			m_editorWorld->Render();

		if (m_gameInstance && !m_PIEWorld.expired() && m_PIEWorld.lock()->m_isVisalbe)
			m_PIEWorld.lock()->Render();
	}

	bool EditorEngine::IsRunning()
	{
		return m_isRunning;
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
		//ASSERT(p_worldContext.CurrentScene != nullptr); can have no current if first browse

		auto destination = m_allLevels.find(p_sceneName);

		//not a valid scene name
		if (destination == m_allLevels.end())
			return false;

		if (PrepareSceneChange(*m_PIEWorld.lock(), destination->second) &&
			CommitSceneChange(*m_PIEWorld.lock(), destination->second))
			return false;

		//update editorWorld level
		m_editorWorld->CurrentScene() = destination->second;
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