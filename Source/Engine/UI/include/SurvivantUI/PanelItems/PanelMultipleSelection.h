//PanelMultipleSelection.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelMultipleSelection : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(int)>;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_selectable">Displayed options</param>
		/// <param name="p_callback">callback with the new selected index as parameter</param>
		PanelMultipleSelection(
			const std::string& p_name,
			const std::vector<std::string>& p_selectable,
			const Callback& p_callback);
		~PanelMultipleSelection() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string GetDisplayString();

		std::string					m_name;
		std::vector<std::string>	m_items;
		int							m_count;
		int							m_curentSelection;
		Callback					m_callback;
	};
}