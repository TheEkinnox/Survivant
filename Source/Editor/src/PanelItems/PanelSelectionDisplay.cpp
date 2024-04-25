//PanelSelectionDisplay.cpp
#include "SurvivantEditor/PanelItems/PanelSelectionDisplay.h"

#include "Arithmetic.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	PanelSelectionDisplay::PanelSelectionDisplay(
		const std::string& p_name, 
		const std::vector<std::string>& p_selectable, 
		const std::vector<SelectedDisplay>& p_selectedDisplay,
		int p_defaultSelection) :
		m_selection(p_name, p_selectable, [this](int p_index) { SetSelection(p_index); }),
		m_selectedDisplay(p_selectedDisplay)
	{
		m_index = LibMath::clamp(p_defaultSelection, 0, static_cast<int>(m_selectedDisplay.size()));
	}

	void PanelSelectionDisplay::DisplayAndUpdatePanel()
	{
		m_selection.DisplayAndUpdatePanel();

		for (auto& item : m_selectedDisplay[m_index])
			item->DisplayAndUpdatePanel();
	}

	void PanelSelectionDisplay::SetSelection(int p_selection)
	{
		m_index = LibMath::clamp(p_selection, 0, static_cast<int>(m_selectedDisplay.size()));
	}
}