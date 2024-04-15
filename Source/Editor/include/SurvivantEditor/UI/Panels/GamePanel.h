//GamePanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelImage.h"
#include "SurvivantEditor/UI/PanelItems/PanelButtonList.h"
#include "SurvivantEditor/App/WorldContext.h"

#include <functional>
#include <cstdint>

namespace SvEditor::UI::Panels
{
	using namespace PanelItems;

	class GamePanel : public Panel
	{
	public:
		GamePanel();
		~GamePanel();

		static void SetGameWorldCreator(const App::WorldContext::WorldCreator& p_worldCreator);

		ERenderFlags Render()override;

		static size_t AddPlayListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddPauseListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddFrameListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);

		static constexpr char NAME[] = "Game";
	private:

		static inline App::WorldContext::WorldCreator		s_worldCreator;
		static inline PanelButton::OnButtonPressEvent		s_playListenners;
		static inline PanelButton::OnButtonPressEvent		s_pauseListenners;
		static inline PanelButton::OnButtonPressEvent		s_frameListenners;

		std::shared_ptr<App::WorldContext> m_world;
		PanelButtonList		m_buttons;
		PanelImage			m_image;
	};

}