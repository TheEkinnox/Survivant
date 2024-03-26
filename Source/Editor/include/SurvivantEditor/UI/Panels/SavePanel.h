//SavePanel.h
#pragma once

#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelButtonList.h"

#include <functional>


namespace SvEditor::UI::Panels
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
