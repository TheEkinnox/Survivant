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
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;
		static bool oldWindowSizeValue = false;
		bool showWindow = true;

		if (ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			m_buttons.DisplayAndUpdatePanel();

			if (IsWindowDifferentSize(m_imageSize, oldWindowSizeValue))
				s_onResizeEvent.Invoke(m_imageSize);

			auto pos = ImGui::GetCursorPos();
			m_imagePos = { ImGui::GetCursorScreenPos().x , ImGui::GetCursorScreenPos().y };

			m_image.DisplayAndUpdatePanel();

			//click 
			if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
			{
				auto mousePos = ImGui::GetMousePos();
				auto val = CalculateUVCords({ mousePos.x, mousePos.y });
				
				if (0 <= val.m_x && val.m_x <= 1 &&
					0 <= val.m_y && val.m_y <= 1)
					s_onClickSceneEvent.Invoke(CalculateUVCords({ mousePos.x, mousePos.y }));
			}
		}

		ImGui::End();


		ERenderFlags flags = ERenderFlags();
		if (!showWindow)
			flags = ERenderFlags(flags | ERenderFlags::CLOSE);

		return flags;
	}

	void ScenePanel::AddClickSceneListenner(const ClickEvent::EventDelegate& p_callback)
	{
		s_onClickSceneEvent.AddListener(p_callback);
	}

	void ScenePanel::AddResizeListenner(const ResizeEvent::EventDelegate& p_callback)
	{
		s_onResizeEvent.AddListener(p_callback);
	}

	void ScenePanel::ToggleTexture()
	{
		static bool isScene = true;

		m_image.SetTexture(isScene? s_idTexture: s_sceneTexture);
		isScene = !isScene;
	}

	LibMath::Vector2 ScenePanel::CalculateUVCords(const LibMath::Vector2& p_cursorPos)
	{
		return (p_cursorPos - m_imagePos) / m_imageSize;
	}
}
