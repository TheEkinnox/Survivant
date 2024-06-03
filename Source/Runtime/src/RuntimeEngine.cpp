//RuntimeEngine.cpp
#include "SurvivantRuntime/RuntimeEngine.h"


#include <SurvivantApp/Core/BuildConfig.h>
#include <SurvivantApp/Core/TempDefaultScene.h>

#include <SurvivantAudio/AudioContext.h>

#include <SurvivantCore/Debug/Assertion.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantRendering/RHI/IRenderAPI.h>

#include <SurvivantScripting/LuaContext.h>


using namespace LibMath;

using namespace SvApp::Core;

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::RHI;
using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::Components;

namespace SvRuntime
{
	void RuntimeEngine::Init()
	{
		s_engine = this;

		//audio
		if (!SvAudio::AudioContext::GetInstance().Init())
			ASSERT(false, "Failed to initialize audio context");

		//physics
		SvPhysics::PhysicsContext::GetInstance().Init();

		//scripts
		SvScripting::LuaContext::SetUserTypeBinders(); //default binders
		SvScripting::LuaContext::GetInstance().Init();

		//create world
		m_world = CreateGameWorld();
		m_game = CreateGameInstance();

		CHECK(InitializeGameInstance(), "couldnt initialize GameInstance");

		m_world->SetInputs();
		//m_camera = EntityHandle(m_world->CurrentScene().Get(), m_world->GetFirstCamera());

		//start game
		StartGame();
	}

	void RuntimeEngine::Update()
	{
		m_time.Tick();

		if (!m_scenePath.empty())
		{
			bool changeSuccess = ChangeSceneInternal();

			if (!CHECK(changeSuccess))
				m_isRunning = false;
		}
	}

	RuntimeEngine::GameInstancePtr RuntimeEngine::CreateGameInstance()
	{
		return std::make_unique<GameInstance>();
	}

	RuntimeEngine::WorldContextPtr RuntimeEngine::CreateGameWorld()
	{
		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);
		world->m_owningGameInstance = nullptr;

		world->m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);


		return world;
	}

	WorldContext::SceneRef RuntimeEngine::GetStartScene()
	{
		using namespace SvApp::Core;
		static std::string configFilePath = "buildConfig.txt";

		auto& rm = ResourceManager::GetInstance();

		auto config = rm.Load<BuildConfig>(configFilePath);
		auto scene = rm.Load<Scene>(config->m_scene.GetPath());

		return scene;
	}

	void RuntimeEngine::UpdateGame()
	{
		SvScripting::LuaContext::GetInstance().Update(GetDeltaTime());   //use engine time 1 timer
		SvPhysics::PhysicsContext::GetInstance().Update(GetDeltaTime());
	}

	bool RuntimeEngine::InitializeGameInstance()
	{
		m_world->m_owningGameInstance = m_game.get();
		m_world->CurrentScene() = GetStartScene();

		m_world->m_inputs = ToRemove::SetupGameInputs();
		m_world->SetCamera(m_world->GetFirstCamera()); //dont use cam un rendering context
		m_world->BakeLighting();
		m_world->SetInputs();

		//init
		m_game->Init(m_world);

		return true;
	}

	bool RuntimeEngine::ChangeSceneInternal()
	{
		std::string scenePath = std::move(m_scenePath);

		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();

		SvPhysics::PhysicsContext::GetInstance().Reload();
		Timer::GetInstance().Refresh();

		//if (m_gameInstance)
		luaContext.Reload();

		//couldnt browse to scene
		if (!BrowseToScene(*m_world, scenePath))
			return false;

		//if (m_gameInstance)
		luaContext.Start();

		return true;
	}

	void RuntimeEngine::StartGame()
	{
		//SvPhysics::PhysicsContext::GetInstance().Reload();
		//Timer::GetInstance().Refresh();
		SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
		//luaContext.Reload();

		//ResourceManager& resourceManager = ResourceManager::GetInstance();
		//resourceManager.ReloadAll<SvScripting::LuaScript>();


		//init
		m_game->Init(m_world);
		m_world->m_owningGameInstance = m_game.get();

		luaContext.Start();
		ASSERT(luaContext.IsValid(), "Failed to start lua context");

		//start
		m_game->Start();
	}

	void RuntimeEngine::Render() const
	{
		//if (m_editorWorld->m_isVisalbe)
		//	m_editorWorld->m_renderingContext->Render(m_editorWorld->CurrentScene().Get());

		//if (m_gameInstance && !m_PIEWorld.expired() && m_PIEWorld.lock()->m_isVisalbe)
		//	m_PIEWorld.lock()->m_renderingContext->Render(m_PIEWorld.lock()->CurrentScene().Get());


		Scene* scene = m_world->CurrentScene().Get();
		Renderer::UpdateLightSSBO(scene, *m_world->m_lightsSSBO);

		Renderer::RenderInfo renderInfo{
			.m_aspect = m_world->m_renderingContext->GetAspect(),
			.m_scene = scene
		};

		m_world->m_renderingContext->GetRenderer().Render(renderInfo);
	}

	void RuntimeEngine::SetViewport(const LibMath::TVector2<int>& p_size)
	{
		IRenderAPI::GetCurrent().SetViewport( { 0, 0 }, p_size);

		//TODO: Cam
		//camera
		//auto cam = m_camera.Get<CameraComponent>();
		//if (cam)
		//	cam->SetAspect(static_cast<float>(p_size.m_x) / static_cast<float>(p_size.m_y));
	}

	void RuntimeEngine::BakeLights()
	{
		m_world->BakeLighting();
	}

	IEngine::SceneRef RuntimeEngine::GetCurrentScene() const
	{
		if (!m_world)
			return {};

		return m_world->m_currentSceneRef ? *m_world->m_currentSceneRef : SceneRef();
	}

	bool RuntimeEngine::IsRunning()
	{
		return m_isRunning;
	}

	void RuntimeEngine::ChangeScene(const std::string& p_scenePath)
	{
		m_scenePath = p_scenePath;
	}

	bool RuntimeEngine::ChangeCamera(const SvCore::ECS::EntityHandle& p_camera)
	{
		if (!p_camera)
			return false;

		m_world->SetCamera(p_camera);

		return true;
	}

	float RuntimeEngine::GetDeltaTime()
	{
		return m_time.GetDeltaTime();
	}
}