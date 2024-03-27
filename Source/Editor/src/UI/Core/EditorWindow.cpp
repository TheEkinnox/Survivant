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
		m_ui = nullptr;

		SvCore::Events::EventManager::GetInstance().AddListenner<SvApp::Window::WindowClosing>(([this]() { m_shouldClose = true; }));
	}

	void EditorWindow::StartRender()
	{
		SvApp::Window::StartRender();
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

	void EditorWindow::SetupUI(EditorUI* p_ui)
	{
		m_ui = p_ui;
		m_ui->InitEditorUi(this);
		p_ui->SetSceneTexture(static_cast<intptr_t>(m_textureId));
	}
}



