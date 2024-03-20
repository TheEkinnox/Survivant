//ScenePanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/PanelItems/PanelImage.h"
#include "SurvivantUI/PanelItems/PanelButtonList.h"

#include <functional>
#include <cstdint>


namespace SvUI::Panels
{
	using namespace PanelItems;

	class ScenePanel : public Panel
	{
	public:
		ScenePanel();
		~ScenePanel();

		static void SetSceneTexture(intptr_t p_texture);

		ERenderFlags Render()override;


	private:
		static constexpr char NAME[] = "Scene";

		static inline SvCore::Utility::UnusedIdGenerator	s_idGenerator;
		static inline intptr_t								s_sceneTexture;

		PanelButtonList		m_buttons;
		PanelImage			m_image;
	};

}