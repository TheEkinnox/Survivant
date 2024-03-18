//PanelButtonList.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/PanelItems/PanelButton.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelButtonList : Interfaces::IPanelable
	{
	public:
		PanelButtonList() = default;
		~PanelButtonList() = default;

		void DisplayAndUpdatePanel()override;

		std::vector<PanelButton> m_buttons;
	};
}