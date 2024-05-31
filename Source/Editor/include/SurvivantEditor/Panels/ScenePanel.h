//ScenePanel.h
#pragma once

#include "SurvivantEditor/Gizmo/SceneGizmos.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelImage.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/Events/Event.h>
#include <SurvivantCore/Utility/UnusedIdGenerator.h>

#include <Vector/Vector2.h>

#include <functional>
#include <cstdint>

namespace SvApp::Core
{
	struct WorldContext;
}

namespace SvEditor::Panels
{
	using namespace PanelItems;

	class ScenePanel : public Panel
	{
	public:
		using ClickEvent = SvCore::Events::Event<const LibMath::Vector2&>;
		using ResizeEvent = SvCore::Events::Event<const LibMath::Vector2&>;

		using WorldContext = SvApp::Core::WorldContext;

		ScenePanel();
		~ScenePanel();

		static void SetSceneWorld(std::weak_ptr<WorldContext> p_world);

		ERenderFlags Render()override;

		/// <summary>
		/// Add listenner to click scene event
		/// </summary>
		/// <param name="p_callback">callback that takes UV cords as param</param>
		static void AddClickSceneListenner(const ClickEvent::EventDelegate& p_callback);

		/// <summary>
		/// Add listenner to resize event
		/// </summary>
		/// <param name="p_callback">callback that takes new size as param</param>
		static void AddResizeListenner(const ResizeEvent::EventDelegate& p_callback);

		static void SelectEntity(const SvCore::ECS::EntityHandle& p_entity);
		static constexpr char NAME[] = "Scene";

	private:
		static inline constexpr float OFFSET = 30;

		void				RenderInfoPanel(bool p_isSmallDisplay);
		void				SetGizmoTransformType(int p_val);
		void				InvokeClickScene();
		LibMath::Vector2	CalculateUVCords(const LibMath::Vector2& p_cursorPos);


		static inline ClickEvent					s_onClickSceneEvent;
		static inline ResizeEvent					s_onResizeEvent;
		static inline std::weak_ptr<WorldContext>	s_world;

		PanelButtonList			m_buttons;
		PanelUniqueSelection	m_transformType;
		PanelImage				m_image;

		LibMath::Vector2	m_imagePos;
		LibMath::Vector2	m_imageSize;
		bool				m_prevFocus;

		Gizmo::SceneGizmos	m_gizmos;
	};
}