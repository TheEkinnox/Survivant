//ScenePanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelImage.h"
#include "SurvivantEditor/UI/PanelItems/PanelButtonList.h"

#include <functional>
#include <cstdint>


namespace SvEditor::UI::Panels
{
	using namespace PanelItems;

	class ScenePanel : public Panel
	{
	public:
		ScenePanel();
		~ScenePanel();

		static void SetSceneTexture(intptr_t p_texture);

		ERenderFlags Render()override;

		static size_t AddPlayListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddPauseListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);
		static size_t AddFrameListenner(const PanelButton::OnButtonPressEvent::EventDelegate& p_callback);

		static void RemovePlayListenner(size_t p_id);
		static void RemovePauseListenner(size_t p_id);
		static void RemoveFrameListenner(size_t p_id);


	private:
		static constexpr char NAME[] = "Scene";

		static inline SvCore::Utility::UnusedIdGenerator	s_idGenerator;
		static inline intptr_t								s_sceneTexture;
		static inline PanelButton::OnButtonPressEvent		s_playListenners;
		static inline PanelButton::OnButtonPressEvent		s_pauseListenners;
		static inline PanelButton::OnButtonPressEvent		s_frameListenners;

		PanelButtonList		m_buttons;
		PanelImage			m_image;
	};

}