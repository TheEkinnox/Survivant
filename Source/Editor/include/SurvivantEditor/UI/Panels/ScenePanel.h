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
		static void SetIdTexture(intptr_t p_texture);

		ERenderFlags Render()override;

	private:
		void ToggleTexture();


		static constexpr char NAME[] = "Scene";

		static inline SvCore::Utility::UnusedIdGenerator	s_idGenerator;
		static inline intptr_t								s_sceneTexture;
		static inline intptr_t								s_idTexture;

		PanelButtonList		m_buttons;
		PanelImage			m_image;
	};
}