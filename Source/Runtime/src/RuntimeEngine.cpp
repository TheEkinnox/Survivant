//RuntimeEngine.cpp
#include "SurvivantRuntime/RuntimeEngine.h"

#include <SurvivantApp/Core/BuildConfig.h>
#include <SurvivantApp/Windows/Window.h>

#include <SurvivantAudio/AudioContext.h>

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Utility/ServiceLocator.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantRendering/RHI/IRenderAPI.h>

#include <SurvivantScripting/LuaContext.h>

using namespace LibMath;

using namespace SvApp;
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
    RuntimeEngine::RuntimeEngine()
    {
        m_resourceManager = std::make_unique<ResourceManager>();
        m_luaContext      = std::make_unique<SvScripting::LuaContext>();
        m_physicsContext  = std::make_unique<SvPhysics::PhysicsContext>();
        m_audioContext    = std::make_unique<SvAudio::AudioContext>();

        ServiceLocator::Provide<Timer>(m_time);
        ServiceLocator::Provide<ResourceManager>(*m_resourceManager);
        ServiceLocator::Provide<SvScripting::LuaContext>(*m_luaContext);
        ServiceLocator::Provide<SvPhysics::PhysicsContext>(*m_physicsContext);
        ServiceLocator::Provide<SvAudio::AudioContext>(*m_audioContext);
    }

    void RuntimeEngine::Init()
    {
        s_engine = this;

        //audio
        if (!m_audioContext->Init())
            ASSERT(false, "Failed to initialize audio context");

        //physics
        m_physicsContext->Init();

        //scripts
        m_luaContext->Init();

        //create world
        m_world = CreateGameWorld();
        m_game  = CreateGameInstance();

        CHECK(InitializeGameInstance(), "couldnt initialize GameInstance");

        //start game
        StartGame();
    }

    RuntimeEngine::~RuntimeEngine()
    {
        m_world.reset();

        SvScripting::LuaContext& luaContext = SvScripting::LuaContext::GetInstance();
        ResourceManager::GetInstance().Clear();
        SvPhysics::PhysicsContext::GetInstance().Reset();

        if (m_game)
        {
            luaContext.Stop();
            m_game.reset();
        }

        luaContext.Reset();
        SvAudio::AudioContext::GetInstance().Reset();
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
        auto world                  = CreateNewWorldContext(WorldContext::EWorldType::NONE);
        world->m_renderingContext = std::make_shared<RenderingContext>(EntityHandle());
        world->m_owningGameInstance = nullptr;

        world->m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);


        return world;
    }

    WorldContext::SceneRef RuntimeEngine::GetStartScene()
    {
        using namespace SvApp::Core;
        static std::string configFilePath = "buildConfig.txt";

        auto& rm = SV_SERVICE(ResourceManager);

        auto config = rm.Load<BuildConfig>(configFilePath);
        auto scene  = rm.Load<Scene>(config->m_scene.GetPath());

        return scene;
    }

    void RuntimeEngine::UpdateGame()
    {
        if (!CHECK(m_luaContext->IsValid(), "LuaContext Not valid"))
        {
            SV_EVENT_MANAGER().Invoke<SvApp::Window::WindowCloseRequest>();
            return;
        }

        m_luaContext->Update(GetDeltaTime());   //use engine time 1 timer
        m_physicsContext->Update(GetDeltaTime());
    }

    bool RuntimeEngine::InitializeGameInstance()
    {
        m_world->m_owningGameInstance = m_game.get();
        m_world->CurrentScene()       = GetStartScene();
        m_world->m_inputs             = std::make_shared<InputManager::InputBindings>();

        m_world->BakeLighting();
        m_world->SetInputs();

        //init
        m_game->Init(m_world);

        return true;
    }

    bool RuntimeEngine::ChangeSceneInternal()
    {
        const std::string scenePath = std::move(m_scenePath);


        m_luaContext->Reload();
        m_physicsContext->Reload();
        m_time.Refresh();

        //couldn't browse to scene
        if (!BrowseToScene(*m_world, scenePath))
            return false;

        m_luaContext->Start();

        return true;
    }

    void RuntimeEngine::StartGame()
    {
        //init
        m_game->Init(m_world);
        m_world->m_owningGameInstance = m_game.get();

        m_luaContext->Start();
        ASSERT(m_luaContext->IsValid(), "Failed to start lua context");

        //start
        m_game->Start();
    }

    void RuntimeEngine::Render() const
    {
        Scene* scene = m_world->CurrentScene().Get();
        Renderer::UpdateLightSSBO(scene, *m_world->m_lightsSSBO);

        m_world->m_renderingContext->Render(scene);
    }

    void RuntimeEngine::SetViewport(const Vector2I& p_size)
    {
        m_world->m_renderingContext->Resize(p_size);
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

    bool RuntimeEngine::IsRunning() const
    {
        return m_isRunning;
    }

    void RuntimeEngine::ChangeScene(const std::string& p_scenePath)
    {
        m_scenePath = p_scenePath;
    }

    bool RuntimeEngine::ChangeCamera(const EntityHandle& p_camera)
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
