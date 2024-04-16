//PanelPopupMenuButton.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/MenuItems/PopupMenu.h"

#include <functional>
#include <string>

namespace SvEditor::PanelItems
{
	using namespace Interfaces;
	using namespace MenuItems;

	class PanelPopupMenuButton : public IPanelable
	{
	public:
		PanelPopupMenuButton(const std::string& p_name) : m_name(p_name) {}
		~PanelPopupMenuButton() = default;
		PanelPopupMenuButton(const PanelPopupMenuButton& p_other);
		PanelPopupMenuButton(PanelPopupMenuButton&& p_other) noexcept;
		PanelPopupMenuButton& operator=(const PanelPopupMenuButton& p_other);

		PanelPopupMenuButton* Clone() const;

		//IPanelable
		void DisplayAndUpdatePanel() override;

		bool IsEmpty() { return m_items.size() == 0; }

		std::vector<std::unique_ptr<IMenuable>> m_items;

	private:
		std::string		m_name;
	};
}