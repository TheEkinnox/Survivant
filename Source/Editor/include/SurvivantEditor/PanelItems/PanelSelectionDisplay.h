//PanelVec3Input.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelSelectionDisplay : public Interfaces::IPanelable
	{
	public:
		using SelectedDisplay = std::vector<std::shared_ptr<Interfaces::IPanelable>>;

		PanelSelectionDisplay(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const std::vector<SelectedDisplay>& p_selectedDisplay,
			int p_defaultSelection = 0);

		virtual void DisplayAndUpdatePanel() override;

		void SetSelection(int p_selection);

	private:

		std::string						m_name;
		PanelUniqueSelection			m_selection;
		int								m_index;
		std::vector<SelectedDisplay>	m_selectedDisplay;
	};
}