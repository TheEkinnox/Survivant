//ScenePanel.cpp

#include "SurvivantEditor/Panels/ScenePanel.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorUI.h"

#include "SurvivantApp/Core/WorldContext.h"

#include "Vector/Vector2.h"

#include "imgui.h"

using namespace SvApp::Core;
using namespace LibMath;
using namespace SvEditor::Gizmo;

namespace SvEditor::Panels
{
	ScenePanel::ScenePanel() :
		m_gizmos(s_world.lock()->m_renderingContext),
		m_displayGizmos("Gizmos ", { "TRANSFORM", "ORIENTATION", "GRID", "COLLIDER" }, 
			PanelMultipleSelection::GetCopyFunc([this]() -> int { return static_cast<int>(m_gizmos.m_displayed); }),
			[this](int p_val) { m_gizmos.m_displayed = SceneGizmos::EGizmoFlag(p_val); }),
		m_transformType("   Transform Type ", { "TRANSLATE", "ROTATE", "SCALE" },
			PanelUniqueSelection::GetRefFunc([]() -> int& { static int val; return val; }),
			[this](int p_val) { SetGizmoTransformType(p_val); }),
		m_gridHeight(   "   Grid Height    ",
			[this]() -> float& { return m_gizmos.m_grid.m_height; })
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
		static float SMALL_DISPLAY = 450;
		bool showWindow = true;

		if (s_world.lock()->m_isVisalbe = ImGui::Begin(m_name.c_str(), &showWindow, window_flags))
		{
			if (IsWindowDifferentSize(m_imageSize))
			{
				s_onResizeEvent.Invoke(m_imageSize);
				m_image.SetTexture(s_world.lock()->m_renderingContext->GetTextureId(
					RenderingContext::ETextureType::COLOR));
			}

			bool isSmallDisplay = m_imageSize.m_x <= SMALL_DISPLAY;
			ImVec2 topLeft = ImVec2(ImGui::GetCursorPosX() + OFFSET, ImGui::GetCursorPosY() + OFFSET);
			m_imagePos = { ImGui::GetCursorScreenPos().x , ImGui::GetCursorScreenPos().y };

			ImGui::SetNextItemAllowOverlap();
			m_image.DisplayAndUpdatePanel();
			bool sceneHovered = ImGui::IsItemHovered();

			m_gizmos.RenderGizmos(Vector2(m_imageSize.m_x - OFFSET, topLeft.y), isSmallDisplay);

			ImGui::SetCursorPos(topLeft);
			ImGui::SetNextItemAllowOverlap();

			RenderInfoPanel(isSmallDisplay);
			sceneHovered &= !ImGui::IsItemHovered();

			//click
			if (!m_gizmos.UsingGizmo() && ImGui::IsMouseClicked(0) &&
				s_world.lock()->m_isFocused && sceneHovered)
				InvokeClickScene();

			//focus
			s_world.lock()->m_isFocused = ImGui::IsWindowFocused();
		}

		ImGui::End();

		ERenderFlags flags = ERenderFlags();
		if (!showWindow)
			flags = ERenderFlags(flags | ERenderFlags::CLOSE);

		return flags;
	}

	void ScenePanel::AddClickSceneListener(const ClickEvent::EventDelegate& p_callback)
	{
		s_onClickSceneEvent.AddListener(p_callback);
	}

	void ScenePanel::AddResizeListener(const ResizeEvent::EventDelegate& p_callback)
	{
		s_onResizeEvent.AddListener(p_callback);
	}

	void ScenePanel::SelectEntity(const SvCore::ECS::EntityHandle& p_entity)
	{
		if (s_world.expired())
			return;

		s_world.lock()->m_renderingContext->s_editorSelectedEntity = p_entity;
	}

	void ScenePanel::InvokeClickScene()
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

	void ScenePanel::RenderInfoPanel(bool p_isSmallDisplay)
	{
		float width = p_isSmallDisplay? ImGui::GetContentRegionAvail().x - OFFSET:
										(ImGui::GetContentRegionAvail().x - OFFSET) * 0.5f - OFFSET;
		if (ImGui::BeginChild("##", ImVec2(width, 0),
			ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse) &&
			ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_CollapsingHeader))
		{
			m_displayGizmos.DisplayAndUpdatePanel();

			if (m_gizmos.m_displayed & SceneGizmos::EGizmoFlag::TRANSFORM)
				m_transformType.DisplayAndUpdatePanel();

			if (m_gizmos.m_displayed & SceneGizmos::EGizmoFlag::GRID)
				m_gridHeight.DisplayAndUpdatePanel();
		}
		ImGui::EndChild();
	}

	void ScenePanel::SetGizmoTransformType(int p_val)
	{
		ImGuizmo::OPERATION operation;

		if (p_val == 0)
			operation = ImGuizmo::OPERATION::TRANSLATE;
		else if (p_val == 1)
			operation = ImGuizmo::OPERATION::ROTATE;
		else if (p_val == 2)
			operation = ImGuizmo::OPERATION::SCALE;

		m_gizmos.m_transform.SetOperation(operation);
	}

	LibMath::Vector2 ScenePanel::CalculateUVCords(const LibMath::Vector2& p_cursorPos)
	{
		return (p_cursorPos - m_imagePos) / m_imageSize;
	}
}
