//EditorWindow.cpp

#include "SurvivantEditor/UI/Core/EditorWindow.h"

#include "SurvivantCore/Events/EventManager.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::UI::Core
{
	EditorWindow::EditorWindow() : SvApp::Window()
	{
		m_shouldClose = 0;
		m_inputs = std::make_shared<SvApp::InputManager::InputBindings>();

		SvCore::Events::EventManager::GetInstance().AddListenner<SvApp::Window::WindowClosing>(([this]() { m_shouldClose = true; }));

		m_ui = std::make_unique<EditorUI>(*m_inputs);
		m_ui->InitEditorUi(this);
	}

	void EditorWindow::Update()
	{
		SvApp::Window::Update();
		m_ui->StartFrameUpdate();
	}

	void EditorWindow::RenderUI()
	{
		m_ui->RenderPanels();
	}

	void EditorWindow::EndRender()
	{
		m_ui->EndFrameUpdate();
		SvApp::Window::EndRender();
	}

	bool EditorWindow::ShouldClose()
	{
		//return Window::ShouldClose();
		return Window::ShouldClose() && m_shouldClose;
	}

	void UI::Core::EditorWindow::SetupUI(const SetupSceneInfo& p_sceneInfo)
	{
		m_ui->InitGamePanel(p_sceneInfo.m_textureId, p_sceneInfo.m_playPauseFrameCallbacks);
	}

	EditorUI& UI::Core::EditorWindow::GetUI()
	{
		return *m_ui;
	}
	std::shared_ptr<SvApp::InputManager::InputBindings> UI::Core::EditorWindow::GetInputs()
	{
		return m_inputs;
	}
}



