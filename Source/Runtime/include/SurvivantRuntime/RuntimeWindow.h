////RuntimeWindow.h
//#pragma once
//
//#include "SurvivantApp/Inputs/InputManager.h"
//#include "SurvivantApp/Windows/Window.h"
//#include "SurvivantCore/Events/Event.h"
//#include "SurvivantApp/Core/WorldContext.h"
//#include "SurvivantEditor/Core/EditorUI.h"
//
//#include <unordered_map>
//#include <array>
//
//
//namespace SvRuntime
//{
//	class RuntimeWindow : public SvApp::Window
//	{
//	public:
//		using WorldContext = SvApp::Core::WorldContext;
//
//		RuntimeWindow();
//		~RuntimeWindow() = default;
//
//		void UpdateScripts() override;
//		void RenderUI();
//		void EndRender() override;
//		bool ShouldClose() override;
//
//		void SetupUI(
//			const std::weak_ptr<WorldContext>&			p_sceneWorld,
//			const WorldContext::WorldCreator&			p_gameWorld,
//			const std::array<std::function<void()>, 3>	p_playPauseFrameCallbacks);
//
//		EditorUI& GetUI();
//
//	private:
//		//init on creation
//		std::unique_ptr<EditorUI>	m_ui;
//		bool						m_shouldClose = false; 
//
//		std::shared_ptr<SvApp::InputManager::InputBindings>		m_inputs;
//	};
//}
