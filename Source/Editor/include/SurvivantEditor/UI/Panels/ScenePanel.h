//ScenePanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelImage.h"
#include "SurvivantEditor/UI/PanelItems/PanelButtonList.h"

#include "Vector/Vector2.h"

#include <functional>
#include <cstdint>

namespace SvEditor::App
{
	struct WorldContext;
}

namespace SvEditor::UI::Panels
{
	using namespace PanelItems;

	class ScenePanel : public Panel
	{
	public:
		using ClickEvent = SvCore::Events::Event<const LibMath::Vector2&>;
		using ResizeEvent = SvCore::Events::Event<const LibMath::Vector2&>;

		ScenePanel();
		~ScenePanel();

		static void SetSceneWorld(std::weak_ptr<App::WorldContext> p_world);

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

		static constexpr char NAME[] = "Scene";

	private:
		void ToggleTexture();
		LibMath::Vector2 CalculateUVCords(const LibMath::Vector2& p_cursorPos);

		static inline ClickEvent							s_onClickSceneEvent;
		static inline ResizeEvent							s_onResizeEvent;
		static inline std::weak_ptr<App::WorldContext>		s_world;

		PanelButtonList		m_buttons;
		PanelImage			m_image;

		LibMath::Vector2	m_imagePos;
		LibMath::Vector2	m_imageSize;
	};
}