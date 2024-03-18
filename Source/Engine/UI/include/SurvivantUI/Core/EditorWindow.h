//EditorWindow.h
#pragma once

#include "SurvivantApp/Windows/Window.h"
#include "SurvivantUI/Core/EditorUI.h"

#include "SurvivantApp/Inputs/InputManager.h"

#include <unordered_map>
#include "SurvivantCore/Events/Event.h"


namespace SvUI::Core
{
	class EditorWindow : public App::Window
	{
	public:
		EditorWindow();
		~EditorWindow() = default;

		void StartRender() override;
		void RenderUI();
		void EndRender() override;
		bool ShouldClose() override;

		void SetupUI(EditorUI* p_ui);

	private:
		using KeyMap = std::unordered_map<App::InputManager::KeyboardKeyType, App::InputManager::KeyCallback>;

		//init on creation
		EditorUI*	m_ui;
		bool		m_shouldClose = false; 
		KeyMap		m_keyInputCallbacks;
	};
}
