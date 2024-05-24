//EngineApp.h
#pragma once

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Core/EditorWindow.h"

#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Core/IApp.h>

using namespace LibMath;

namespace SvEditor::Core
{
	class EngineApp : public SvApp::Core::IApp
	{
	public:
		using GameInstance = SvApp::Core::GameInstance;

		EngineApp();
		~EngineApp() override;

		// Inherited via IApp
		void Init() override;
		void Run() override;

	private:
		void LoadAllResources();

		void TogglePlayPIE();
		void TogglePausePIE();
		void PressFramePIE();
		void UpdatePIE();

		void UpdateScripts();
		void UpdatePhysics();

		//class GameLoopEvent : public SvCore::Events::Event<> {};

		std::unique_ptr<Core::EditorWindow>			m_window;
		bool										m_gameIsPaused;
		std::weak_ptr<GameInstance>					m_gameInstance;
		EditorEngine								m_editorEngine;
	};
}