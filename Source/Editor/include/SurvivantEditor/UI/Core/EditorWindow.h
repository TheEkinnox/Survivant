//EditorWindow.h
#pragma once

#include "SurvivantApp/Windows/Window.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"

#include "SurvivantApp/Inputs/InputManager.h"

#include <unordered_map>
#include "SurvivantCore/Events/Event.h"


namespace SvEditor::UI::Core
{
	class EditorWindow : public SvApp::Window
	{
	public:
		EditorWindow(intptr_t p_sceneTexture);
		~EditorWindow() = default;

		void Update() override;
		void RenderUI();
		void EndRender() override;
		bool ShouldClose() override;

		void SetupUI(intptr_t p_sceneTexture);

		EditorUI& GetUI();

	private:
		using KeyMap = std::unordered_map<SvApp::InputManager::KeyboardKeyType, SvApp::InputManager::KeyCallback>;

		//init on creation
		std::unique_ptr<EditorUI>	m_ui;
		bool						m_shouldClose = false; 
		KeyMap						m_keyInputCallbacks;

	};
}
