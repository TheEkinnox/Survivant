//ScenePanel.cpp

#include "SurvivantEditor/Panels/ScenePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorUI.h"

#include "SurvivantApp/Core/WorldContext.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace SvApp::Core;

namespace SvEditor::Panels
{
	ScenePanel::ScenePanel() :
		m_gizmos(s_world.lock()->m_renderingContext)
	{
		m_name = NAME;

		m_image.SetTexture(s_world.lock()->m_renderingContext->GetTextureId(RenderingContext::ETextureType::COLOR));
		//m_buttons.m_buttons.push_back(PanelButton("Toogle Texture", [this]() { ToggleTexture(); }));
	}

	ScenePanel::~ScenePanel()
	{}

	void ScenePanel::SetSceneWorld(std::weak_ptr<WorldContext> p_world)
	{
		s_world = p_world;
	}

	Panel::ERenderFlags ScenePanel::Render()
	{
		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs;
		bool showWindow = true;


		if (s_world.lock()->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			//focus
			s_world.lock()->m_isFocused = ImGui::IsWindowFocused();

			//panelables
			m_buttons.DisplayAndUpdatePanel();

			if (IsWindowDifferentSize(m_imageSize))
			{
				s_onResizeEvent.Invoke(m_imageSize);
				m_image.SetTexture(s_world.lock()->m_renderingContext->GetTextureId(
					RenderingContext::ETextureType::COLOR));
			}

			auto pos = ImGui::GetCursorPos();
			m_imagePos = { ImGui::GetCursorScreenPos().x , ImGui::GetCursorScreenPos().y };

			ImGui::SetNextItemAllowOverlap();
			m_image.DisplayAndUpdatePanel();

			//click 
			if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
			{
				auto mousePos = ImGui::GetMousePos();
				auto uv = CalculateUVCords({ mousePos.x, mousePos.y });
				
				if (0 <= uv.m_x && uv.m_x <= 1 &&
					0 <= uv.m_y && uv.m_y <= 1)
				{
					uv.m_y = 1.0f - uv.m_y;
					s_onClickSceneEvent.Invoke(uv);
				}
			}

			m_gizmos.RenderGizmos();
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

	void ScenePanel::SelectEntity(const SvCore::ECS::EntityHandle& p_entity)
	{
		if (s_world.expired())
			return;

		s_world.lock()->m_renderingContext->s_editorSelectedEntity = p_entity;
	}

	LibMath::Vector2 ScenePanel::CalculateUVCords(const LibMath::Vector2& p_cursorPos)
	{
		return (p_cursorPos - m_imagePos) / m_imageSize;
	}
}
