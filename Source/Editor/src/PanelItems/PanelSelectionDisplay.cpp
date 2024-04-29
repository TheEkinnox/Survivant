//PanelSelectionDisplay.cpp
#include "SurvivantEditor/PanelItems/PanelSelectionDisplay.h"

#include "Arithmetic.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	PanelSelectionDisplay::PanelSelectionDisplay(
		const std::string& p_name, const std::vector<std::string>& p_selectionNames, 
		const std::vector<SelectedDisplay>& p_selectedDisplay, int& p_currentSelection, 
		const Callback& p_callback) :
		PanelSelectionDisplay(
			p_name, p_selectionNames,
			p_selectedDisplay, 
			GetRefFunc([p_currentSelection]() mutable -> int& { return p_currentSelection; }),
			p_callback)
	{}

	PanelSelectionDisplay::PanelSelectionDisplay(
		const std::string& p_name, const std::vector<std::string>& p_selectionNames, 
		const std::vector<SelectedDisplay>& p_selectedDisplay, const GetRefFunc& p_getRef, 
		const Callback& p_callback) : 
		PanelInputBase(p_getRef),
		m_selection(p_name, p_selectionNames, p_getRef, p_callback),
		m_selectedDisplay(p_selectedDisplay)
	{}

	PanelSelectionDisplay::PanelSelectionDisplay(
		const std::string & p_name, const std::vector<std::string>&p_selectionNames,
		const std::vector<SelectedDisplay>&p_selectedDisplay, const GetCopyFunc & p_getCopy, 
		const Callback & p_callback) :
		PanelInputBase(p_getCopy, Callback()),
		m_selection(p_name, p_selectionNames, p_getCopy, p_callback),
		m_selectedDisplay(p_selectedDisplay)
	{
	}

	void PanelSelectionDisplay::DisplayAndUpdatePanel()
	{
		m_selection.DisplayAndUpdatePanel();

		for (auto& item : m_selectedDisplay[GetRef()])
			item->DisplayAndUpdatePanel();
	}
}