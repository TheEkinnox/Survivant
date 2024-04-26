//PanelVec3Input.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelSelectionDisplay : public PanelInputBase<int>
	{
	public:
		using SelectedDisplay = std::vector<std::shared_ptr<Interfaces::IPanelable>>;

		PanelSelectionDisplay(
			const std::string& p_name,
			const std::vector<std::string>& p_selectionNames,
			const std::vector<SelectedDisplay>& p_selectedDisplay,
			int& p_currentSelection,
			const Callback& p_callback = Callback());
		PanelSelectionDisplay(
			const std::string& p_name,
			const std::vector<std::string>& p_selectionNames,
			const std::vector<SelectedDisplay>& p_selectedDisplay,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelSelectionDisplay(
			const std::string& p_name,
			const std::vector<std::string>& p_selectionNames,
			const std::vector<SelectedDisplay>& p_selectedDisplay,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback = Callback());

		virtual void DisplayAndUpdatePanel() override;

	private:
		PanelUniqueSelection			m_selection;
		std::vector<SelectedDisplay>	m_selectedDisplay;
	};
}