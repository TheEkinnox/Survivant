//PanelButtonList.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/PanelItems/PanelButton.h"

#include <functional>

namespace SvEditor::UI::PanelItems
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