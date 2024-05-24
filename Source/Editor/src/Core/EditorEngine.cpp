//EditorEngine.cpp
#include "SurvivantEditor/Core/EditorEngine.h"

#include "SurvivantEditor/Core/EditorWindow.h"
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include <SurvivantApp/Core/TempDefaultScene.h>

#include <SurvivantCore/Debug/Assertion.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantScripting/LuaContext.h>

using namespace SvApp::Core;

namespace SvEditor::Core
{
	void EditorEngine::Init()
	{
		s_engine = this;

		//create scenes
		//BrowseToDefaultScene(*m_editorWorld); //cant use this func bcs world does not exist
		m_editorSelectedScene = ResourceManager::GetInstance().GetOrCreate<Scene>(DEFAULT_SCENE_PATH);

		//create editor world world
		m_editorWorld = CreateEditorDefaultWorld(m_editorSelectedScene);
		m_editorWorld->SetInputs();

		//setup events
		SetupEditorEvents();
	}

	void EditorEngine::Update()
	{
		m_time.Tick();

		if (m_editorWorld->m_isFocused)
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

		if (!SaveSceneState())
			return {};

		SvPhysics::PhysicsContext::GetInstance().Reload();
		Timer::GetInstance().Refresh();
		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		luaContext.Reload();

		auto& pieWorld = *m_PIEWorld.lock();
		pieWorld.CurrentScene() = ResourceManager::GetInstance().Load<Scene>(m_editorSelectedScene.GetPath());
		pieWorld.m_inputs = std::make_shared<SvApp::InputManager::InputBindings>();
		pieWorld.SetCamera(pieWorld.GetFirstCamera());
		pieWorld.SetInputs();
		pieWorld.BakeLighting();

		//init
		m_gameInstance = std::make_shared<GameInstance>();
		m_gameInstance->Init(m_PIEWorld);
		pieWorld.m_owningGameInstance = m_gameInstance.get();

		luaContext.Start();
		if (!CHECK(luaContext.IsValid(), "Failed to start lua context"))
			return {};

		return std::weak_ptr<GameInstance>(m_gameInstance);
	}

	void EditorEngine::DestroyGameInstance()
	{
		SvPhysics::PhysicsContext::GetInstance().Reload();
		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		luaContext.Stop();
		luaContext.Reload();

		ASSERT(luaContext.IsValid());

		WorldContext::SceneRef scene = GetWorldContextRef(*m_gameInstance).lock()->CurrentScene();
		m_gameInstance.reset();

		RestoreSceneState();

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
		k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				m_editorWorld->m_renderingContext->CameraMoveInput().m_y += 1;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				m_editorWorld->m_renderingContext->CameraMoveInput().m_y -= 1;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::PRESSED, EInputModifier::MOD_ANY }, [this](const char)
			{
				++m_editorWorld->m_renderingContext->CameraRotateInput().m_x;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
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
		cam.SetPerspective(90_deg, .01f, 50.f);
		cam.SetClearColor(Color::lightGray);
		world->SetCamera(cam, Transform({ 0.f, 0.f, 0.f }, Quaternion::identity(), Vector3::one()));
		world->m_inputs = CreateEditorInputs();

		//load and render
		//world->Save();
		world->BakeLighting();
		world->m_renderingContext->Render(world->CurrentScene().Get());

		//world->m_persistentLevel = nullptr;

		return world;
	}

	void SvEditor::Core::EditorEngine::SetupEditorEvents()
	{
		using namespace RuntimeBuild;

		SV_EVENT_MANAGER().AddListenner<OnCreateBuildGame>(OnCreateBuildGame::EventDelegate(
			[](const std::string& p_buildFileName,
				const SvApp::Core::BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateBuild(p_buildFileName, p_buildInfo);
			}));

		SV_EVENT_MANAGER().AddListenner<OnCreateBuildAndRun>(OnCreateBuildAndRun::EventDelegate(
			[](const std::string& p_buildFileName,
				const SvApp::Core::BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateAndRunBuild(p_buildFileName, p_buildInfo);
			}));

		SV_EVENT_MANAGER().AddListenner<OnSave>(OnSave::EventDelegate(
			[this]()
			{ 
				auto& engine = *this;
				if (engine.IsPlayInEditor())
				{
					SV_LOG_WARNING("Can't save scene during play mode");
					return;
				}

				OnSave::s_saveSucceded = m_editorWorld->Save(true);
				if (OnSave::s_saveSucceded)
				{
					m_isEditorModifiedScene = false;
					SV_LOG("Scene successfully saved to \"%s\"", m_editorWorld->CurrentScene().GetFullPath().c_str());
				}
			}));

		SV_EVENT_MANAGER().AddListenner<OnEditorModifiedScene>(OnEditorModifiedScene::EventDelegate(
			[this]()
			{ 
				if (!this->IsPlayInEditor())
					m_isEditorModifiedScene = true;
			}));
	}

	std::string EditorEngine::GetTemporaryScenePath() const
	{
		if (!m_editorSelectedScene)
			return {};

		std::ostringstream oss;
		oss << m_editorSelectedScene.GetFullPath() << ".tmp";
		return oss.str();
	}

	bool EditorEngine::SaveSceneState() const
	{
		if (!m_editorSelectedScene)
			return false;

		std::error_code       err;
		static constexpr auto options = std::filesystem::copy_options::overwrite_existing;

		std::filesystem::copy(m_editorSelectedScene.GetFullPath(), GetTemporaryScenePath(), options, err);

		if (!CHECK(err.value() == 0, "Failed to copy scene state from \"%s\" to \"%s\" - %s",
				m_editorSelectedScene.GetFullPath().c_str(), GetTemporaryScenePath().c_str(), err.message().c_str()))
			return false;

		SV_EVENT_MANAGER().Invoke<OnSave>();
		return OnSave::s_saveSucceded;
	}

	bool EditorEngine::RestoreSceneState()
	{
		if (!m_editorSelectedScene)
			return false;

		m_editorSelectedScene = ResourceManager::GetInstance().Load<Scene>(m_editorSelectedScene.GetPath());

		static constexpr auto options = std::filesystem::copy_options::overwrite_existing;

		const std::string tempPath = GetTemporaryScenePath();
		std::error_code   err;

		std::filesystem::copy(tempPath, m_editorSelectedScene.GetFullPath(), options, err);

		return CHECK(err.value() == 0, "Failed to restore scene state from \"%s\" - %s", tempPath.c_str(), err.message().c_str())
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

	bool SvEditor::Core::EditorEngine::IsEditorModifiedScene()
	{
		return m_isEditorModifiedScene;
	}

	bool SvEditor::Core::EditorEngine::IsGameFocused()
	{
		return !m_PIEWorld.expired() && m_PIEWorld.lock()->m_isFocused;
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
		auto& world = m_gameInstance? *m_PIEWorld.lock() : *m_editorWorld;

		if (!m_gameInstance && m_isEditorModifiedScene && p_scenePath != world.CurrentScene().GetPath())
			SV_EVENT_MANAGER().Invoke<OnSave>();
		
		//couldnt browse to scene
		if (!BrowseToScene(world, p_scenePath))
			return false;

		//update editorWorld level. Dont bcs change back
		if (!m_gameInstance)
			m_editorSelectedScene = m_editorWorld->CurrentScene();

		m_isEditorModifiedScene = false;

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