//PanelUniqueSelection.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelUniqueSelection : public PanelInputBase<int>
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_selectable">Displayed options</param>
		/// <param name="p_callback">callback with all selected flags as a parameter</param>
		PanelUniqueSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			int& p_currentSelection,
			const Callback& p_callback = Callback());
		PanelUniqueSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelUniqueSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);

		~PanelUniqueSelection() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
		std::string		m_items;
		int				m_count;
	};
}
