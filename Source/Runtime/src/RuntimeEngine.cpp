//RuntimeEngine.cpp
#include "SurvivantRuntime/RuntimeEngine.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantRuntime/RuntimeWindow.h"

#include "SurvivantApp/Core/TempDefaultScene.h"



namespace SvRuntime
{
	void RuntimeEngine::Init()
	{
		s_engine = this;

		//create scenes
		//BrowseToDefaultScene(*m_editorWorld); //cant use this func bcs world does not exist

		//create editor world world
		//m_game = std::make_unique<GameInstance>(m_world);
		m_world = CreateGameWorld();
		m_game = CreateGameInstance();

		CHECK(InitializeGameInstance(), "couldnt initialize gi");

		m_world->SetInputs();
	}

	void RuntimeEngine::Update()
	{
		m_time.Tick();

		//m_editorWorld->m_renderingContext->UpdateCameraInput();

		//if (m_editorWorld)
		//	m_editorWorld->RenderContext();

		//if (!m_PIEWorld.expired())
		//	m_PIEWorld.lock()->RenderContext();
	}

	RuntimeEngine::GameInstancePtr RuntimeEngine::CreateGameInstance()
	{
		return std::make_unique<GameInstance>(m_world);
	}

	RuntimeEngine::WorldContextPtr RuntimeEngine::CreateGameWorld()
	{
		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);
		world->m_owningGameInstance = nullptr;

		world->m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
		world->m_viewport = GetViewportSize();
		world->CurrentScene() = GetStartScene();
		world->m_inputs = nullptr;
		world->m_renderingContext = nullptr;
		
		world->BakeLighting();
		world->m_renderingContext->Render(world->CurrentScene().Get());

		return world;
	}

	WorldContext::SceneRef RuntimeEngine::GetStartScene()
	{
		ASSERT(false, "GetStartScene not implemented");
		return WorldContext::SceneRef();
	}

	LibMath::TVector2<int> RuntimeEngine::GetViewportSize()
	{
		ASSERT(false, "GetViewportSize not implemented");
		return LibMath::TVector2<int>();
	}

	void RuntimeEngine::UpdateGame()
	{
		m_game->Update();
	}

	bool RuntimeEngine::InitializeGameInstance()
	{
		m_world->m_owningGameInstance = m_game.get();
		m_world->CurrentScene() = GetStartScene();

		m_world->m_inputs = ToRemove::SetupGameInputs();
		m_world->SetSceneCamera();
		m_world->BakeLighting();
		m_world->SetInputs();

		//init
		m_game->Init();

		return true;
	}

	void RuntimeEngine::Render()
	{
		RenderingContext::DefaultFBGameRendering(m_camera);
	}

	void RuntimeEngine::BakeLights()
	{
		m_world->BakeLighting();
	}

	bool RuntimeEngine::IsRunning()
	{
		return m_isRunning;
	}

	bool RuntimeEngine::ChangeScene(const std::string& p_scenePath)
	{
		//couldnt browse to scene
		if (!BrowseToScene(*m_world, p_scenePath))
			return false;

		return true;
	}

	void RuntimeEngine::RedrawViewports(int p_width, int p_height)
	{
		m_world->m_viewport = { p_width , p_height };

		//camera
		auto cam = m_camera.Get<CameraComponent>();

		if (cam)
			cam->SetAspect(static_cast<float>(p_width) / static_cast<float>(p_width));
	}

	float RuntimeEngine::GetDeltaTime()
	{
		return m_time.GetDeltaTime();
	}

	bool RuntimeEngine::IsPlayInEditor()
	{
		return false;
	}
}