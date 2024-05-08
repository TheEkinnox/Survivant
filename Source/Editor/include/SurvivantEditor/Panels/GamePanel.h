//GamePanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelImage.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantApp/Core/WorldContext.h"

#include <Vector/Vector2.h>

#include <functional>
#include <cstdint>

namespace SvEditor::Panels
{
	using namespace PanelItems;

	class GamePanel : public Panel
	{
	public:
		using WorldContext = SvApp::Core::WorldContext;
		using ResizeEvent = SvCore::Events::Event<const LibMath::Vector2&>;

		GamePanel();
		~GamePanel();

		static void SetGameWorldCreator(const WorldContext::WorldCreator& p_worldCreator);

		ERenderFlags Render()override;

		static size_t AddPlayListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddPauseListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddFrameListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);

		static constexpr char NAME[] = "Game";
	private:
		static inline WorldContext::WorldCreator			s_worldCreator;
		static inline PanelButton::OnButtonPressEvent		s_playListenners;
		static inline PanelButton::OnButtonPressEvent		s_pauseListenners;
		static inline PanelButton::OnButtonPressEvent		s_frameListenners;

		std::shared_ptr<WorldContext>	m_world;
		PanelButtonList					m_buttons;
		PanelImage						m_image;
		bool							m_prevFocus;
		LibMath::Vector2				m_imageSize;
		ResizeEvent						m_onResize;
	};

}