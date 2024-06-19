//RuntimeApp.cpp
#include "SurvivantRuntime/RuntimeApp.h"

#include "SurvivantRuntime/ComponentRegistrations.h"
#include "SurvivantRuntime/ResourceRegistrations.h"

#include <SurvivantApp/Inputs/InputManager.h>
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantCore/Utility/FileSystem.h>

#include <memory>

using namespace LibMath;

using namespace SvApp;

using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::Enums;
using namespace SvRendering::RHI;

namespace SvRuntime
{
    RuntimeApp::RuntimeApp()
        : m_runtimeEngine(std::make_unique<RuntimeEngine>()), m_gameIsPaused(false)
    {
    }

    RuntimeApp::~RuntimeApp()
    {
        m_runtimeEngine.reset();
        m_window.reset();
    }

    void RuntimeApp::Init()
    {
        using namespace SvApp;

        m_gameIsPaused = false;

#ifdef _DEBUG
        SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
#endif

        SV_SERVICE(ResourceManager).AddSearchPath("assets");

        const bool result = SetWorkingDirectory(GetApplicationDirectory());
        ASSERT(result, "Failed to update working directory");
        (void)result;

        m_window = std::make_unique<Window>();

        IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
        renderAPI.Init(true)
                 .SetCapability(ERenderingCapability::DEPTH_TEST, true)
                 .SetCullFace(ECullFace::BACK);

        InputManager::GetInstance().InitWindow(m_window.get());

        m_runtimeEngine->Init();

        SV_EVENT_MANAGER().AddListener<Window::OnFrameBufferSize>([this](int p_width, int p_height)
        {
            m_runtimeEngine->SetViewport({ p_width, p_height });
        });

        Vector2I viewport;
        m_window->GetSize(viewport.m_x, viewport.m_y);
        m_runtimeEngine->SetViewport(viewport);
    }

    void RuntimeApp::Run()
    {
        while (!m_window->ShouldClose() && m_runtimeEngine->IsRunning())
        {
            m_runtimeEngine->Update();
            m_window->Update();
            InputManager::GetInstance().Update();

            if (!m_gameIsPaused)
                m_runtimeEngine->UpdateGame();

            m_runtimeEngine->Render();

            m_window->EndRender();
        }
    }
}
