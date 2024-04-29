//PanelButtonList.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelButtonList : Interfaces::IPanelable
	{
	public:
		PanelButtonList() = default;
		~PanelButtonList() = default;

		void DisplayAndUpdatePanel()override;

		std::vector<PanelButton> m_buttons;

		bool m_gotClicked;
	};
}