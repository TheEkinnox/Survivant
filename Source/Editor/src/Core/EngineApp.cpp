#include "SurvivantEditor/Core/EngineApp.h"


#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"

#include <memory>

#include "SurvivantApp/Core/TempDefaultScene.h"

namespace SvEditor::Core
{
	EngineApp::EngineApp() :
		m_gameIsPaused(false)
	{
		m_gameInstance = std::weak_ptr<GameInstance>();
	}

	EngineApp::~EngineApp()
	{
		m_window.release();
	}

	void EngineApp::Init()
	{
		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
		ResourceManager::GetInstance().AddSearchPath("assets");

		//setup InspectorComponents
		InspectorItemManager::Init();

		m_window = std::make_unique<Core::EditorWindow>();

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		//SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

		m_editorEngine.Init();

		LoadAllResources();
		SvApp::InputManager::GetInstance().InitWindow(m_window.get());

		m_editorEngine.SetupUI(m_window.get(), {
			[this]() { TogglePlayPIE(); },
			[this]() { TogglePausePIE(); },
			[this]() { PressFramePIE(); }
			});
	}

	void EngineApp::Run()
	{
		while (!m_window->ShouldClose() && m_editorEngine.IsRunning())
		{
			m_editorEngine.Update();
			m_window->Update();
			SvApp::InputManager::GetInstance().Update();

			if (!m_gameInstance.expired() && !m_gameIsPaused)
			{
				m_gameInstance.lock()->Update();
			}

			m_editorEngine.RenderWorlds();

			m_window->RenderUI();
			m_window->EndRender();
		}
	}

	void EngineApp::LoadAllResources()
	{
		//TODO : make/use resource manager init
	}

	void EngineApp::TogglePlayPIE()
	{
		if (!m_gameInstance.expired()) //game is running
		{
			m_editorEngine.DestroyGameInstance();
			m_window->GetUI().ForceSceneFocus();
		}
		else //game not running
		{
			m_gameInstance = m_editorEngine.CreatePIEGameInstance();
			m_gameInstance.lock()->Start();
			m_window->GetUI().ForceGameFocus();
			//this is tmp game
		}
	}

	void EngineApp::TogglePausePIE()
	{
		//TODO: toggle pause
	}

	void EngineApp::PressFramePIE()
	{
		//TODO: press frame
	}

	void EngineApp::PauseGameInstance()
	{

	}

}

