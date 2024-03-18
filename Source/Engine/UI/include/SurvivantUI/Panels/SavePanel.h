//SavePanel.h
#pragma once

#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/PanelItems/PanelButtonList.h"

#include <functional>


namespace SvUI::Panels
{
	using namespace PanelItems;

	class SavePanel : public Panel
	{
	public:
		SavePanel();
		~SavePanel();

		ERenderFlags Render() override;

		static int GetPanelCount() { return s_panelCount; };

	private:
		static constexpr char NAME[] = "Save";

		static inline int s_panelCount = 0;

		PanelButtonList m_options;
		bool			m_open = true;
	};
}
