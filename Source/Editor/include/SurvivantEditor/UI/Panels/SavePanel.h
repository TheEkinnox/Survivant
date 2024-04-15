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

		static constexpr char NAME[] = "Save";
	private:

		PanelButtonList m_options;
		bool			m_open = true;
	};
}
