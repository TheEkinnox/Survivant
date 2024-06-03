//EngineApp.h
#pragma once

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Core/EditorWindow.h"

#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Core/IApp.h>

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
		void DrawLogo();

		void TogglePlayPIE();
		void TogglePausePIE();
		void PressFramePIE();
		void UpdatePIE();

		void UpdateScripts();
		void UpdatePhysics();

		//class GameLoopEvent : public SvCore::Events::Event<> {};

		std::unique_ptr<EditorWindow>			m_window;
		std::weak_ptr<GameInstance>				m_gameInstance;
		EditorEngine							m_editorEngine;
	};
}