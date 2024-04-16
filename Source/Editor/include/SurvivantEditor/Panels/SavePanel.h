//SavePanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"

#include <functional>


namespace SvEditor::Panels
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
