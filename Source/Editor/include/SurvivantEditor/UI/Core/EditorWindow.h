//EditorWindow.h
#pragma once

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Windows/Window.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/App/WorldContext.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"

#include <unordered_map>
#include <array>


namespace SvEditor::UI::Core
{
	class EditorWindow : public SvApp::Window
	{
	public:
		EditorWindow();
		~EditorWindow() = default;

		void Update() override;
		void RenderUI();
		void EndRender() override;
		bool ShouldClose() override;

		void SetupUI(
			const std::weak_ptr<App::WorldContext>&		p_sceneWorld,
			const App::WorldContext::WorldCreator&		p_gameWorld,
			const std::array<std::function<void()>, 3>	p_playPauseFrameCallbacks);

		EditorUI& GetUI();
		std::shared_ptr<SvApp::InputManager::InputBindings> GetInputs();

	private:
		using KeyMap = std::unordered_map<SvApp::InputManager::KeyboardKeyType, SvApp::InputManager::KeyCallback>;

		//init on creation
		std::unique_ptr<EditorUI>	m_ui;
		bool						m_shouldClose = false; 
		KeyMap						m_keyInputCallbacks;

		std::shared_ptr<SvApp::InputManager::InputBindings>		m_inputs;
	};
}
