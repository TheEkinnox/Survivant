//GamePanel.cpp

#include "SurvivantEditor/UI/Panels/GamePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"
#include "SurvivantEditor/App/WorldContext.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::Panels
{
	GamePanel::GamePanel()
	{
		m_name = NAME;

		m_buttons.m_buttons.reserve(3);
		m_buttons.m_buttons.push_back(PanelButton(" Start |> ", s_playListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Pause || ", s_pauseListenners));
		m_buttons.m_buttons.push_back(PanelButton(" Frame -> ", s_frameListenners));

		m_world = s_worldCreator({ 0,0 });
		m_image.SetTexture(m_world->GetDefaultTextureId());
	}

	GamePanel::~GamePanel()
	{
	}

	void GamePanel::SetGameWorldCreator(const App::WorldContext::WorldCreator& p_worldCreator)
	{
		s_worldCreator = p_worldCreator;
	}

	Panel::ERenderFlags GamePanel::Render()
	{
		static intptr_t tmp = 1;
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;
		bool showWindow = true;

		//ImGuiFocusedFlags_;

		//if (ImGui::IsWindowFocused())

		if (ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			m_world->Render();

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
}
