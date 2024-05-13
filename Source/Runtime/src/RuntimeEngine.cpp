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
		m_camera = EntityHandle(m_world->CurrentScene().Get(), m_world->GetFirstCamera());

		m_game->Start();
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
		return std::make_unique<GameInstance>();
	}

	RuntimeEngine::WorldContextPtr RuntimeEngine::CreateGameWorld()
	{
		auto world = CreateNewWorldContext(WorldContext::EWorldType::EDITOR);
		world->m_owningGameInstance = nullptr;

		world->m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
		world->m_viewport = { 0, 0 };

		
		return world;
	}

	WorldContext::SceneRef RuntimeEngine::GetStartScene()
	{
		ASSERT(false, "GetStartScene not implemented");
		return WorldContext::SceneRef();
	}

	void RuntimeEngine::UpdateGame()
	{
		m_game->UpdateScripts();
	}

	bool RuntimeEngine::InitializeGameInstance()
	{
		m_world->m_owningGameInstance = m_game.get();
		//m_world->CurrentScene() = GetStartScene();
		BrowseToDefaultScene(*m_world);

		m_world->m_inputs = ToRemove::SetupGameInputs();
		m_world->SetCamera(m_world->GetFirstCamera()); //dont use cam un rendering context
		m_world->BakeLighting();
		m_world->SetInputs();

		//init
		m_game->Init(m_world);

		return true;
	}

	void RuntimeEngine::Render()
	{
		RenderingContext::DefaultFBGameRendering(m_camera);
	}

	void RuntimeEngine::SetViewport(const LibMath::TVector2<int>& p_size)
	{
		m_world->m_viewport = p_size;

		IRenderAPI::GetCurrent().SetViewport( { 0, 0 }, p_size);

		//camera
		auto cam = m_camera.Get<CameraComponent>();

		if (cam)
			cam->SetAspect(static_cast<float>(p_size.m_x) / static_cast<float>(p_size.m_y));
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

	bool RuntimeEngine::ChangeCamera(const SvCore::ECS::EntityHandle& p_camera)
	{
		auto cam = p_camera.Get<CameraComponent>();
		auto trans = p_camera.Get<Transform>();

		if (!(cam && trans))
			SV_LOG("Can't change camera, Entity doesn't have a camera or transform component");

		m_camera = p_camera;

		return false;
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