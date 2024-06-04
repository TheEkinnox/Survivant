//RuntimeWindow.cpp

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorWindow.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::Core
{
	EditorWindow::EditorWindow() : SvApp::Window()
	{
		m_shouldClose = 0;

		SvCore::Events::EventManager::GetInstance().AddListener<SvApp::Window::WindowClosing>(([this]() { m_shouldClose = true; }));

		m_ui = std::make_unique<EditorUI>();
		m_ui->InitWindow(this);
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

	void Core::EditorWindow::SetupUI(
		const std::weak_ptr<WorldContext>& p_sceneWorld,
		const WorldContext::WorldCreator& p_gameWorld,
		const std::array<std::function<void()>, 3>	p_playPauseFrameCallbacks)
	{
		m_ui->InitScenePanel(p_sceneWorld);
		m_ui->InitHierarchyPanel(p_sceneWorld);
		m_ui->InitGamePanel(p_gameWorld, p_playPauseFrameCallbacks);
	}

	EditorUI& Core::EditorWindow::GetUI()
	{
		return *m_ui;
	}

	void EditorWindow::RenderLogo()
	{
		Update();
		m_ui->RenderLogo();
		EndRender();
	}
}



