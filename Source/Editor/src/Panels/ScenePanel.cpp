//ScenePanel.cpp

#include "SurvivantEditor/Panels/ScenePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorUI.h"

#include "SurvivantApp/Core/WorldContext.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::Panels
{
	ScenePanel::ScenePanel()
	{
		m_name = NAME;

		m_image.SetTexture(s_world.lock()->m_renderingContext->GetDefaultTextureId());
		m_buttons.m_buttons.push_back(PanelButton("Toogle Texture", [this]() { ToggleTexture(); }));
	}

	ScenePanel::~ScenePanel()
	{
	}

	void ScenePanel::SetSceneWorld(std::weak_ptr<WorldContext> p_world)
	{
		s_world = p_world;
	}

	Panel::ERenderFlags ScenePanel::Render()
	{
		static intptr_t tmp = 1;
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;
		static bool oldWindowSizeValue = false;
		bool showWindow = true;

		if (s_world.lock()->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			//focus
			//auto val = IsGainedFocus(m_prevFocus);
			//if (val == 1)
			//	s_world.lock()->m_onGainFocus.Invoke();
			//else if (val == -1)
			//	s_world.lock()->m_onLoseFocus.Invoke();

			//render
			//s_world.lock()->Render();

			//panelables
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
				auto uv = CalculateUVCords({ mousePos.x, mousePos.y });
				
				if (0 <= uv.m_x && uv.m_x <= 1 &&
					0 <= uv.m_y && uv.m_y <= 1)
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

	}

	LibMath::Vector2 ScenePanel::CalculateUVCords(const LibMath::Vector2& p_cursorPos)
	{
		return (p_cursorPos - m_imagePos) / m_imageSize;
	}
}
