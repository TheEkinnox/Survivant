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

using namespace SvCore::Resources;
using namespace SvCore::Utility;
using namespace SvRendering::Enums;
using namespace SvRendering::RHI;

namespace SvRuntime
{
	RuntimeApp::RuntimeApp() :
		m_gameIsPaused(false)
	{
	}

	RuntimeApp::~RuntimeApp()
	{
		m_window.reset();
	}

	void RuntimeApp::Init()
	{
		using namespace SvApp;

		m_gameIsPaused = false;

		//TODO: does this need to be in runtime?
		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
		ResourceManager::GetInstance().AddSearchPath("assets");

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");

		m_window = std::make_unique<Window>();

		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);


		SV_EVENT_MANAGER().AddListenner<Window::OnFrameBufferSize>([this](int p_width, int p_height) {
			m_runEngine.SetViewport({ p_width, p_height }); });

		SvApp::InputManager::GetInstance().InitWindow(m_window.get());

		//LoadAllResources();
		m_window->GetWindow();

		TVector2<int> viewport;
		m_window->GetSize(viewport.m_x, viewport.m_y);
		m_runEngine.SetViewport(viewport);

		m_runEngine.Init();
	}

	void RuntimeApp::Run()
	{
		while (!m_window->ShouldClose() && m_runEngine.IsRunning())
		{
			m_runEngine.Update();
			m_window->Update();
			SvApp::InputManager::GetInstance().Update();

			if (!m_gameIsPaused)
				m_runEngine.UpdateGame();

			m_runEngine.Render();

			m_window->EndRender();
		}
	}
}

