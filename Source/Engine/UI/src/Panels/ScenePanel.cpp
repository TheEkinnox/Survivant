//ScenePanel.cpp

#include "SurvivantUI/Panels/ScenePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantUI/Core/EditorUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
{
	ScenePanel::ScenePanel()
	{
		ASSERT(s_sceneTexture != 0, "Set ScenePanel's scene texture with static function: SetSceneTexture()");

		m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());
		m_image.SetTexture(s_sceneTexture);

		//TODO : Add ScenePanel button callbacks
		m_buttons.m_buttons.reserve(3);
		m_buttons.m_buttons.push_back(PanelButton(" Start |> "));
		m_buttons.m_buttons.push_back(PanelButton(" Pause || "));
		m_buttons.m_buttons.push_back(PanelButton(" Frame -> "));
	}

	ScenePanel::~ScenePanel()
	{
	}

	void ScenePanel::SetSceneTexture(intptr_t p_texture)
	{
		s_sceneTexture = p_texture;
	}

	Panel::ERenderFlags ScenePanel::Render()
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

}
