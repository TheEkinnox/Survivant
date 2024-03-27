//EngineApp.h
#pragma once

#include "SurvivantApp/Core/IApp.h"

#include "SurvivantEditor/UI/Core/EditorWindow.h"

namespace SvEditor::App
{
	class EngineApp : public SvApp::Core::IApp
	{
	public:
		EngineApp() = default;
		~EngineApp()override = default;

		// Inherited via IApp
		void Init() override;
		void Run() override;

	private:
		UI::Core::EditorWindow* m_window;

	};
}