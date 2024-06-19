#include "SurvivantTest/TestApp.h"

#include "SurvivantTest/ResourceRegistrations.h"
#include "SurvivantTest/SceneTest.h"

#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantRendering/Core/Renderer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>

#include <SurvivantScripting/LuaContext.h>
#include <SurvivantScripting/LuaScriptList.h>

using namespace LibMath;

using namespace SvApp;

using namespace SvCore::ECS;
using namespace SvCore::Events;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::Core;
using namespace SvRendering::RHI;
using namespace SvRendering::Enums;
using namespace SvRendering::Resources;
using namespace SvRendering::Components;

using namespace SvPhysics;

using namespace SvScripting;

using Material = SvRendering::Resources::Material;

constexpr const char* TEST_SCENE_PATH = "scenes/test_no_scripts.scn";

constexpr float  CAM_MOVE_SPEED     = 3.f;
constexpr Radian CAM_ROTATION_SPEED = 90_deg;

constexpr size_t TEST_SCRIPTS_COUNT = 256;

constexpr int WINDOW_WIDTH  = 1024;
constexpr int WINDOW_HEIGHT = 768;

namespace SvTest
{
    TestApp::TestApp()
    {
        m_resizeListenerId = EventManager::GetInstance().AddListener<Window::OnFrameBufferSize>(
            [this](const int p_width, const int p_height)
            {
                m_windowSize = { p_width, p_height };
                IRenderAPI::GetCurrent().SetViewport(PosT::zero(), m_windowSize);
            }
        );

        ServiceLocator::Provide<Timer>(m_timer);
    }

    TestApp::~TestApp()
    {
        EventManager::GetInstance().RemoveListener<Window::OnFrameBufferSize>(m_resizeListenerId);
    }

    void TestApp::Init()
    {
        SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
        ResourceManager::GetInstance().AddSearchPath("assets");

        const bool result = SetWorkingDirectory(GetApplicationDirectory());
        ASSERT(result, "Failed to update working directory");
        (void)result;

        SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

        m_window = std::make_unique<Window>("Survivant - Test", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        m_windowSize = { WINDOW_WIDTH, WINDOW_HEIGHT };

        IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
        renderAPI.Init(true)
                 .SetCapability(ERenderingCapability::DEPTH_TEST, true)
                 .SetCullFace(ECullFace::BACK);

        SetupInput();

        m_renderer   = std::make_unique<Renderer>();
        m_lightsSSBO = Renderer::SetupLightSSBO(nullptr);
    }

    void TestApp::Run()
    {
        RunSceneTests();

        PhysicsContext& physicsContext = SV_SERVICE(PhysicsContext);
        physicsContext.Init();

        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Init();

        m_timer.Refresh();
        MakeScene();

        InputManager& input = InputManager::GetInstance();

        while (!m_window->ShouldClose())
        {
            input.Update();

            m_timer.Tick();
            m_window->Update();

            const float deltaTime = m_timer.GetDeltaTime();

            luaContext.Update(deltaTime);
            physicsContext.Update(deltaTime);

            DrawScene();

            m_window->EndRender();
        }

        physicsContext.Reset();
        luaContext.Stop();
        luaContext.Reset();
    }

    void TestApp::SetupInput()
    {
        InputManager& im = InputManager::GetInstance();
        im.InitWindow(m_window.get());
        im.SetInputBindings(std::make_shared<InputManager::InputBindings>());

        auto& keyCallbacks = im.m_bindings->m_keyCallbacks;

        keyCallbacks[{ EKey::W, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::W, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::S, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::S, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::A, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::A, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::D, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::D, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::UP, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::UP, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::R, EKeyState::RELEASED, {} }] = [this](const char)
        {
            MakeScene();
        };
    }

    void TestApp::MakeScene()
    {
        SV_SERVICE(PhysicsContext).Reload();

        m_timer.Refresh();

        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Reload();

        m_scene = ResourceManager::GetInstance().Load<Scene>(TEST_SCENE_PATH);

        for (size_t i = 0; i < TEST_SCRIPTS_COUNT; ++i)
        {
            LuaScriptList& scripts = m_scene->Create().Make<LuaScriptList>();
            scripts.Add("scripts/test.lua");

            ASSERT(luaContext.IsValid());
        }

        luaContext.Start();
    }

    void TestApp::DrawScene() const
    {
        Scene* scene = m_scene.Get();
        Renderer::UpdateLightSSBO(scene, *m_lightsSSBO);

        Renderer::RenderInfo renderInfo{
            .m_aspect = static_cast<float>(m_windowSize.m_x) / static_cast<float>(m_windowSize.m_y),
            .m_scene = scene
        };

        m_renderer->Render(renderInfo);
    }
}
