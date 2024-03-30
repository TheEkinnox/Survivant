//EditorWindow.cpp

#include "SurvivantEditor/UI/Core/EditorWindow.h"

#include "SurvivantCore/Events/EventManager.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::UI::Core
{
	EditorWindow::EditorWindow(intptr_t p_sceneTexture) : SvApp::Window()
	{
		m_shouldClose = 0;

		SvCore::Events::EventManager::GetInstance().AddListenner<SvApp::Window::WindowClosing>(([this]() { m_shouldClose = true; }));

		m_ui = std::make_unique<EditorUI>();
		m_ui->InitEditorUi(this);
		m_ui->SetSceneTexture(p_sceneTexture);
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

	void UI::Core::EditorWindow::SetupUI(intptr_t p_sceneTexture)
	{
		m_ui->SetSceneTexture(p_sceneTexture);
	}

	EditorUI& UI::Core::EditorWindow::GetUI()
	{
		return *m_ui;
	}
}



