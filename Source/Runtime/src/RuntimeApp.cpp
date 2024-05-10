//RuntimeApp.cpp
#include "SurvivantRuntime/RuntimeApp.h"

#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>

#include <memory>

#include "SurvivantApp/Core/TempDefaultScene.h"

namespace SvRuntime
{
	RuntimeApp::RuntimeApp() :
		m_gameIsPaused(false)
	{
	}

	RuntimeApp::~RuntimeApp()
	{
		m_window.release();
	}

	void RuntimeApp::Init()
	{
		using namespace SvApp;

		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
		ResourceManager::GetInstance().AddSearchPath("assets");

		m_window = std::make_unique<Window>(Window());

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");

		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

		SvApp::InputManager::GetInstance().InitWindow(m_window.get());

		//LoadAllResources();

		//Start Game
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
				m_runEngine.Update();


			m_runEngine.Render();

			//m_window->RenderUI();
			m_window->EndRender();
		}
	}
}

