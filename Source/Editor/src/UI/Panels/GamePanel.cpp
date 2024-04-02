//GamePanel.cpp

#include "SurvivantEditor/UI/Panels/GamePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::Panels
{
	GamePanel::GamePanel() :
		m_image(s_gameTexture)
	{
		m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());

		//TODO : Add GamePanel button callbacks
		m_buttons.m_buttons.reserve(3);
		m_buttons.m_buttons.push_back(PanelButton(" Start |> ", s_playListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Pause || ", s_pauseListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Frame -> ", s_frameListenners));
	}

	GamePanel::~GamePanel()
	{
	}

	void GamePanel::SetGameTexture(intptr_t p_texture)
	{
		s_gameTexture = p_texture;
	}

	Panel::ERenderFlags GamePanel::Render()
	{
		static intptr_t tmp = 1;
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
		bool showWindow = true;

		if (ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			m_buttons.DisplayAndUpdatePanel();
			m_image.DisplayAndUpdatePanel();
		}

		ImGui::End();


		ERenderFlags flags = ERenderFlags();
		if (!showWindow)
			flags = ERenderFlags(flags | ERenderFlags::CLOSE);

		return flags;
	}

	size_t GamePanel::AddPlayListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_playListenners.AddListener(p_callback);
	}

	size_t GamePanel::AddPauseListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_pauseListenners.AddListener(p_callback);

	}

	size_t GamePanel::AddFrameListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback)
	{
		return s_frameListenners.AddListener(p_callback);
	}

	//void GamePanel::RemovePlayListenner(size_t p_id)
	//{
	//	s_playListenners.RemoveListener(p_id);

	//}

	//void GamePanel::RemovePauseListenner(size_t p_id)
	//{
	//	s_pauseListenners.RemoveListener(p_id);
	//}

	//void GamePanel::RemoveFrameListenner(size_t p_id)
	//{
	//	s_frameListenners.RemoveListener(p_id);
	//}
}
