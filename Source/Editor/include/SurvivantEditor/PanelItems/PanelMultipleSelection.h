//PanelMultipleSelection.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelMultipleSelection : public PanelInputBase<int>
	{
	public:
		/// <summary>
		///
		/// </summary>
		/// <param name="p_selectable">Displayed options</param>
		/// <param name="p_callback">callback with the new selected index as parameter</param>
		PanelMultipleSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			int& p_currentSelection,
			const Callback& p_callback = Callback());
		PanelMultipleSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelMultipleSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);

		void DisplayAndUpdatePanel() override;

	private:
		std::string GetDisplayString(int p_selection);

		std::string					m_name;
		std::vector<std::string>	m_items;
		std::string					m_displayString;
	};
}