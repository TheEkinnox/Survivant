//EngineApp.h
#pragma once

#include "SurvivantApp/Core/IApp.h"

#include "SurvivantEditor/UI/Core/EditorWindow.h"
#include <SurvivantCore/Utility/Timer.h>
#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/App/EditorEngine.h"

#include <SurvivantRendering/RHI/IRenderAPI.h>



namespace SvEditor::App
{
	class EngineApp : public SvApp::Core::IApp
	{
	public:
		//events
		class OnPIE : public SvCore::Events::Event<GameInstance&> {};
		OnPIE m_onPlayInEditor;
		OnPIE m_onStopInEditor;


		EngineApp();
		~EngineApp() override = default;

		// Inherited via IApp
		void Init() override;
		void Run() override;

	private:
		static SvRendering::RHI::IRenderAPI& SetupRenderAPI();

		void LoadAllResources();
		void SetupEditorInputs();
		void TogglePIE();

		//void CreatePIEGameInstance();
		//void StartPIEGameInstance();

		void PauseGameInstance();

		class GameLoopEvent : public SvCore::Events::Event<> {};

		std::unique_ptr<UI::Core::EditorWindow>		m_window;
		
		//m_renderAPI(SetupRenderAPI())
		//SvRendering::RHI::IRenderAPI&				m_renderAPI;

		bool					m_isRunning;
		bool					m_gameIsPaused;
		SvCore::Utility::Timer	m_time;

		GameInstance*	m_gameInstance;
		GameLoopEvent	m_gameLoop;
		EditorEngine	m_editorEngine;
	};
}