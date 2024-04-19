#include "SurvivantEditor/Core/EngineApp.h"


#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Core/EditorUI.h"

#include <memory>

#include "SurvivantApp/Core/TempDefaultScene.h"

namespace SvEditor::Core
{
	EngineApp::EngineApp() :
		m_gameIsPaused(false)
	{
		m_gameInstance = std::weak_ptr<GameInstance>();
	}

	void EngineApp::Init()
	{
		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
		ResourceManager::GetInstance().AddSearchPath("assets");

		m_window = std::make_unique<Core::EditorWindow>();

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

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

		//using namespace SvEditor;
		//Panels::4Panel::AddClickSceneListenner(
		//	[](const LibMath::Vector2& p_uv)
		//	{
		//		ToRemove::g_idFrameBuffer->Bind();
		//		uint32_t val;
		//		IRenderAPI::GetCurrent().ReadPixels(
		//			p_uv * LibMath::Vector2(800, 600), DimensionsT(1, 1), EPixelDataFormat::RED_INT, EPixelDataType::INT, &val);
		//		ToRemove::g_idFrameBuffer->Unbind();

		//		SV_EVENT_MANAGER().Invoke<Core::EditorUI::DebugEvent>(SvCore::Utility::FormatString("Val = %u", val).c_str());
		//		//SV_EVENT_MANAGER().Invoke<Core::EditorUI::DebugEvent>(SvCore::Utility::FormatString("UV = %f, %f", p_uv.m_x, p_uv.m_y).c_str());
		//	}
		//);
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
			m_onStopInEditor.Invoke(*m_gameInstance.lock());
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

