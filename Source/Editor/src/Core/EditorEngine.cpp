//EditorEngine.cpp
#include "SurvivantEditor/Core/EditorEngine.h"

#include "SurvivantEditor/Core/EditorWindow.h"
#include "SurvivantEditor/Core/LuaEditorBinder.h"
#include "SurvivantEditor/Panels/HierarchyPanel.h"
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include <SurvivantAudio/AudioContext.h>

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Utility/ServiceLocator.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantScripting/LuaContext.h>

using namespace LibMath;

using namespace SvApp::Core;

using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::RHI;
using namespace SvRendering::Core;
using namespace SvRendering::Components;
using namespace SvRendering::Enums;

namespace SvEditor::Core
{
    EditorEngine::EditorEngine()
    {
        m_audioContext    = std::make_unique<SvAudio::AudioContext>();

        ServiceLocator::Provide<Timer>(m_gameTime);
        ServiceLocator::Provide<SvAudio::AudioContext>(*m_audioContext);
    }

    EditorEngine::~EditorEngine()
    {
        m_editorWorld.reset();

		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		ResourceManager::GetInstance().Clear();
		SvPhysics::PhysicsContext::GetInstance().Reset();

		if (m_gameInstance)
		{
			luaContext.Stop();
			m_gameInstance.reset();
		}

		luaContext.Reset();
        m_audioContext.reset();
    }

	void EditorEngine::Init()
	{
		s_engine = this;

        //audio
        if (!m_audioContext->Init())
            ASSERT(false, "Failed to initialize audio context");

		//physics
		SvPhysics::PhysicsContext::GetInstance().Init();

		//scripts
		SvScripting::LuaContext::SetUserTypeBinders(&LuaEditorBinder::EditorUserTypeBindings);
		SvScripting::LuaContext::GetInstance().Init();

		//create scenes
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

		if (!m_scenePath.empty())
		{
			bool changeSuccess = ChangeSceneInternal();

			if (m_gameInstance && !CHECK(changeSuccess))
			{
				SetPaused(false);
				DestroyGameInstance();
			}
			else if (!m_gameInstance)
				ASSERT(changeSuccess, "ChangeScene in Editor failed");
		}

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
		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		luaContext.Reload();
        m_gameTime.Refresh();

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.ReloadAll<SvScripting::LuaScript>();

		auto& pieWorld = *m_PIEWorld.lock();
		pieWorld.CurrentScene() = resourceManager.Load<Scene>(m_editorSelectedScene.GetPath());
		pieWorld.SetCamera(pieWorld.GetFirstCamera());
		pieWorld.SetInputs();
		pieWorld.BakeLighting();

		//init
		m_gameInstance = std::make_shared<GameInstance>();
		m_gameInstance->Init(m_PIEWorld);
		pieWorld.m_owningGameInstance = m_gameInstance.get();

		luaContext.Start();
		if (!CHECK(luaContext.IsValid(), "Failed to start lua context"))
		{
			m_gameInstance.reset();

			luaContext.Reload();
			CHECK(RestoreSceneState(), "Failed to restore pre-play scene state");
			return {};
		}

		return { m_gameInstance };
	}

	void EditorEngine::DestroyGameInstance()
	{
		SvPhysics::PhysicsContext::GetInstance().Reload();
		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		luaContext.Stop();
		luaContext.Reload();
        m_audioContext->StopAll();

		ASSERT(luaContext.IsValid());

		WorldContext::SceneRef scene = GetWorldContextRef(*m_gameInstance).lock()->CurrentScene();
		m_gameInstance.reset();

		CHECK(RestoreSceneState(), "Failed to restore pre-play scene state");

		//while playing, loaded new scene, so go back to selected
		if (scene != m_editorSelectedScene)
		{
			ChangeScene(m_editorSelectedScene.GetPath());
			ChangeSceneInternal();
		}

		m_editorWorld->SetInputs();
		*m_PIEWorld.lock()->m_currentSceneRef = SceneRef();
	}

	std::shared_ptr<WorldContext> EditorEngine::CreatePIEWorld()
	{
		auto pieWorld =				IEngine::CreateNewWorldContext(WorldContext::EWorldType::PIE);
		pieWorld->m_lightsSSBO =	IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, Renderer::LIGHT_SSBO_INDEX);

		return pieWorld;
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
				++m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
			});

		k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::RELEASED, EInputModifier::MOD_ANY }, [this](const char)
			{
				--m_editorWorld->m_renderingContext->CameraMoveInput().m_y;
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
		world->BakeLighting();
		world->m_renderingContext->Render(world->CurrentScene().Get());

		return world;
	}

	void EditorEngine::SetupEditorEvents()
	{
		using namespace RuntimeBuild;

		SV_EVENT_MANAGER().AddListener<OnCreateBuildGame>(OnCreateBuildGame::EventDelegate(
			[](const std::string& p_buildFileName,
				const BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateBuild(p_buildFileName, p_buildInfo);
			}));

		SV_EVENT_MANAGER().AddListener<OnCreateBuildAndRun>(OnCreateBuildAndRun::EventDelegate(
			[](const std::string& p_buildFileName,
				const BuildConfig& p_buildInfo)
			{
				BuildManager::GetInstance().CreateAndRunBuild(p_buildFileName, p_buildInfo);
			}));

		SV_EVENT_MANAGER().AddListener<OnSave>(OnSave::EventDelegate(
			[this]
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

		SV_EVENT_MANAGER().AddListener<OnEditorModifiedScene>(OnEditorModifiedScene::EventDelegate(
			[this]
			{
				if (!this->IsPlayInEditor())
					m_isEditorModifiedScene = true;
			}));
	}

	bool EditorEngine::ChangeSceneInternal()
	{
		HierarchyPanel::ToggleSelectable(SvCore::ECS::NULL_ENTITY.GetIndex());
		const std::string scenePath = std::move(m_scenePath);

		if (!m_gameInstance && m_isEditorModifiedScene && scenePath != m_editorWorld->CurrentScene().GetPath())
			SV_EVENT_MANAGER().Invoke<OnSave>();

		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
        m_gameTime.Refresh();

		SvPhysics::PhysicsContext::GetInstance().Reload();
		Timer::GetInstance().Refresh();
		luaContext.Reload();

		//couldnt browse to scene
		if (!BrowseToScene(*m_editorWorld, scenePath))
			return false;

		if (m_gameInstance)
		{
			CommitSceneChange(*m_PIEWorld.lock(), m_editorWorld->CurrentScene());
			luaContext.Start();
		}
		else //update editorWorld level. Dont bcs change back
			m_editorSelectedScene = m_editorWorld->CurrentScene();

		m_isEditorModifiedScene = false;

		return true;
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
		if (m_editorWorld->m_isVisible)
			m_editorWorld->m_renderingContext->Render(m_editorWorld->CurrentScene().Get());

		if (!m_PIEWorld.expired() && m_PIEWorld.lock()->m_isVisible)
			m_PIEWorld.lock()->m_renderingContext->Render(m_PIEWorld.lock()->CurrentScene().Get());
	}

	bool EditorEngine::IsRunning()
	{
		return m_isRunning;
	}

	bool EditorEngine::IsEditorModifiedScene()
	{
		return m_isEditorModifiedScene;
	}

	void EditorEngine::TogglePause()
	{
		m_isPaused = !m_isPaused;
	}

	void EditorEngine::SetPaused(const bool p_state)
	{
		m_isPaused = p_state;
	}

	bool EditorEngine::IsPaused() const
	{
		return m_isPaused;
	}

	bool EditorEngine::IsGameFocused()
	{
		return !m_PIEWorld.expired() && m_PIEWorld.lock()->m_isFocused;
	}

	void EditorEngine::SetupUI(EditorWindow* p_window, const std::array<std::function<void()>, 3>p_playPauseFrameCallbacks)
	{
		p_window->SetupUI(
			m_editorWorld,
			[this](const Vector2I&)
			{
				ASSERT(m_PIEWorld.expired(), "Tried to create PIE world when already exists");

				auto PIEWorld =	CreatePIEWorld();
				m_PIEWorld = std::weak_ptr(PIEWorld);

				return PIEWorld;
			},
			p_playPauseFrameCallbacks);
	}

	IEngine::SceneRef EditorEngine::GetCurrentScene() const
	{
		auto& world = m_gameInstance ? *m_PIEWorld.lock() : *m_editorWorld;
		return world.CurrentScene();
	}

	void EditorEngine::ChangeScene(const std::string& p_scenePath)
	{
		m_scenePath = p_scenePath;
	}

	float EditorEngine::GetDeltaTime()
	{
		return m_time.GetDeltaTime();
	}

	bool EditorEngine::IsPlayInEditor() const
	{
		return m_gameInstance.get();
	}

	bool EditorEngine::ChangeCamera(const SvCore::ECS::EntityHandle& p_camera)
	{
		if (!p_camera)
			return false;

		m_PIEWorld.lock()->SetCamera(p_camera);

		return true;
	}
}