//EditorEngine.cpp
#include "SurvivantEditor/Core/EditorEngine.h"

#include "SurvivantEditor/Core/EditorWindow.h"
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include <SurvivantApp/Core/TempDefaultScene.h>

#include <SurvivantCore/Debug/Assertion.h>

using namespace SvApp::Core;

namespace SvEditor::Core
{
	void EditorEngine::Init()
	{
		using namespace RuntimeBuild;

		s_engine = this;

		//create scenes
		//BrowseToDefaultScene(*m_editorWorld); //cant use this func bcs world does not exist
		m_editorSelectedScene = ResourceManager::GetInstance().GetOrCreate<Scene>(DEFAULT_SCENE_PATH);

		//create editor world world
		m_editorWorld = CreateEditorDefaultWorld(m_editorSelectedScene);
		m_editorWorld->SetInputs();

		//setup events
		SV_EVENT_MANAGER().AddListenner<OnCreateBuildGame>(OnCreateBuildGame::EventDelegate(
			[](	const std::string& p_buildFileName,
				const SvApp::Core::BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateBuild(p_buildFileName, p_buildInfo);
			}));

		SV_EVENT_MANAGER().AddListenner<OnCreateBuildAndRun>(OnCreateBuildAndRun::EventDelegate(
			[](	const std::string& p_buildFileName,
				const SvApp::Core::BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateAndRunBuild(p_buildFileName, p_buildInfo);
			}));
	}

	void EditorEngine::Update()
	{
		m_time.Tick();
		m_editorWorld->m_renderingContext->UpdateCameraInput();
	}

	std::weak_ptr<GameInstance> EditorEngine::CreatePIEGameInstance()
	{
		//if dont have PIE world context
		if (m_PIEWorld.expired())
		{
			ASSERT(false, "No PIE world on game instance creation");
			m_PIEWorld = CreatePIEWorld();
		}

		m_gameInstance = std::make_shared<GameInstance>();

		if (!InitializePlayInEditorGameInstance())
			return std::weak_ptr<GameInstance>();

		return std::weak_ptr<GameInstance>(m_gameInstance);
	}

	void EditorEngine::DestroyGameInstance()
	{
		WorldContext::SceneRef scene = GetWorldContextRef(*m_gameInstance).lock()->CurrentScene();
		m_gameInstance.reset();

		if (!RestoreSceneState())
			SV_LOG_ERROR("Pre-play scene state couldn't be restored");

		//while playing, loaded new scene, so go back to selected
		if (scene != m_editorSelectedScene)
			BrowseToScene(*m_editorWorld, m_editorSelectedScene.GetPath());

		m_editorWorld->SetInputs();

		m_PIEWorld.lock()->m_renderingContext->Render(m_PIEWorld.lock()->CurrentScene().Get());
	}

	std::shared_ptr<WorldContext> EditorEngine::CreatePIEWorld()
	{
		auto pieWorld =				IEngine::CreateNewWorldContext(WorldContext::EWorldType::PIE);
		pieWorld->m_lightsSSBO =	IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, Renderer::LIGHT_SSBO_INDEX);
		// pieWorld->m_viewport =		p_context.m_viewport;

		//pieWorld->RenderContext();
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

		std::shared_ptr<EditorEngine::Inputs> input = std::make_shared<EditorEngine::Inputs>();

		auto& k = input->m_keyCallbacks;

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

	std::shared_ptr<WorldContext> EditorEngine::CreateEditorDefaultWorld(const WorldContext::SceneRef& p_inScene)
	{
		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);
		world->m_owningGameInstance = nullptr;

		world->m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, Renderer::LIGHT_SSBO_INDEX);
		world->CurrentScene() = p_inScene;
		CameraComponent cam;
		cam.SetPerspective(90_deg, .01f, 14.f);
		cam.SetClearColor(Color::lightGray);
		world->SetCamera(cam, Transform({ 0.f, 1.8f, 2.f }, Quaternion::identity(), Vector3::one()));
		world->m_inputs = CreateEditorInputs();

		//load and render
		//world->Save();
		world->BakeLighting();
		world->m_renderingContext->Render(world->CurrentScene().Get());

		//world->m_persistentLevel = nullptr;

		return world;
	}

	bool EditorEngine::InitializePlayInEditorGameInstance()
	{
		auto& pieWorld = *m_PIEWorld.lock();

		pieWorld.m_owningGameInstance = m_gameInstance.get();

		if (!SaveSceneState())
			return false;

		pieWorld.CurrentScene() = m_editorSelectedScene;

		pieWorld.m_inputs = ToRemove::SetupGameInputs();
		pieWorld.SetCamera(pieWorld.GetFirstCamera());
		pieWorld.SetInputs();
		pieWorld.BakeLighting();

		//init
		m_gameInstance->Init(m_PIEWorld);

		return true;
	}

	std::string EditorEngine::GetTemporaryScenePath() const
	{
		if (!m_editorSelectedScene)
			return {};

		std::ostringstream oss;
		oss << m_editorSelectedScene.GetFullPath() << std::format(".{:%m%d%Y-%H%M}.tmp", std::chrono::system_clock::now());
		return oss.str();
	}

	bool EditorEngine::SaveSceneState() const
	{
		if (!m_editorSelectedScene)
			return false;

		return m_editorSelectedScene.Export(false, GetTemporaryScenePath());
	}

	bool EditorEngine::RestoreSceneState()
	{
		if (!m_editorSelectedScene)
			return false;

		const std::string tempPath = GetTemporaryScenePath();
		std::error_code   err;

		return m_editorSelectedScene->Load(tempPath)
			&& CHECK(std::filesystem::remove(tempPath, err) || err.value() == 0,
				"Failed to remove temporary scene data at \"%s\" - %s", tempPath.c_str(), err.message().c_str());
	}

	void EditorEngine::BakeLights()
	{
		m_editorWorld->BakeLighting();
	}

	void EditorEngine::RenderWorlds()
	{
		if (m_editorWorld->m_isVisalbe)
			m_editorWorld->m_renderingContext->Render(m_editorWorld->CurrentScene().Get());

		if (m_gameInstance && !m_PIEWorld.expired() && m_PIEWorld.lock()->m_isVisalbe)
			m_PIEWorld.lock()->m_renderingContext->Render(m_PIEWorld.lock()->CurrentScene().Get());
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

				auto PIEWorld =	CreatePIEWorld();
				m_PIEWorld = std::weak_ptr<WorldContext>(PIEWorld);

				return PIEWorld;
			},
			p_playPauseFrameCallbacks);
	}

	bool EditorEngine::ChangeScene(const std::string& p_scenePath)
	{
		//ASSERT(!m_allLevels.empty(), "No levels to browse to");
		//ASSERT(p_worldContext.CurrentScene != nullptr); can have no current if first browse

		//auto& rm = ResourceManager::GetInstance();
		auto& world = m_gameInstance? *m_PIEWorld.lock() : *m_editorWorld;

		//couldnt browse to scene
		if (!BrowseToScene(world, p_scenePath))
			return false;

		//update editorWorld level. Dont bcs change back
		if (m_gameInstance)
			m_editorSelectedScene = m_PIEWorld.lock()->CurrentScene();

		//m_editorWorld->CurrentScene() = destination;
		//dont update selected editor scene

		return true;
	}

	float EditorEngine::GetDeltaTime()
	{
		return m_time.GetDeltaTime();
	}

	bool SvEditor::Core::EditorEngine::IsPlayInEditor()
	{
		return m_gameInstance.get();
	}

	bool SvEditor::Core::EditorEngine::ChangeCamera(const SvCore::ECS::EntityHandle& p_camera)
	{
		if (!p_camera)
			return false;

		m_PIEWorld.lock()->SetCamera(p_camera);

		return true;
	}
}