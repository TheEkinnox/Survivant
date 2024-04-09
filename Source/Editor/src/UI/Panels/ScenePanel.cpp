//ScenePanel.cpp

#include "SurvivantEditor/UI/Panels/ScenePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::Panels
{
	ScenePanel::ScenePanel() :
		m_image(s_sceneTexture)
	{
		m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());

		m_buttons.m_buttons.push_back(PanelButton("Toogle Texture", [this]() { ToggleTexture(); }));
	}

	ScenePanel::~ScenePanel()
	{
	}

	void ScenePanel::SetSceneTexture(intptr_t p_texture)
	{
		s_sceneTexture = p_texture;
	}

	void ScenePanel::SetIdTexture(intptr_t p_texture)
	{
		s_idTexture = p_texture;
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
	void ScenePanel::ToggleTexture()
	{
		static bool isScene = true;

		m_image.SetTexture(isScene? s_idTexture: s_sceneTexture);
		isScene = !isScene;
	}
}
