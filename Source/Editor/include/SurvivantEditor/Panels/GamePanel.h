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
		~GamePanel() override = default;

		static void SetGameWorldCreator(const WorldContext::WorldCreator& p_worldCreator);

		ERenderFlags Render() override;

		static size_t AddPlayListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddPauseListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddFrameListener(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);

		static constexpr char NAME[] = "Game";

	private:
		static inline WorldContext::WorldCreator			s_worldCreator;
		static inline PanelButton::OnButtonPressEvent		s_playListeners;
		static inline PanelButton::OnButtonPressEvent		s_pauseListeners;
		static inline PanelButton::OnButtonPressEvent		s_frameListeners;

		std::shared_ptr<WorldContext>	m_world;
		std::unique_ptr<IPanelable>		m_buttons[3];
		PanelImage						m_image;
		bool							m_prevFocus;
		SvApp::ECursorMode				m_prevCursorMode;
		LibMath::Vector2				m_imageSize;
		ResizeEvent						m_onResize;
	};

}