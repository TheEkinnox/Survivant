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
		struct SetupSceneInfo
		{
			intptr_t m_textureId;
			std::function<void()> m_playPauseFrameCallbacks[3];
		};

		EditorWindow();
		~EditorWindow() = default;

		void Update() override;
		void RenderUI();
		void EndRender() override;
		bool ShouldClose() override;

		void SetupUI(const SetupSceneInfo& p_sceneInfo);

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
