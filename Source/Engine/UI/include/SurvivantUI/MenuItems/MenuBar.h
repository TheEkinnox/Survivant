//MenuBar.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/MenuItems/Menu.h"

#include <vector>

using namespace SvUI::Interfaces;

namespace SvUI::MenuItems
{
	class MenuBar : public IPanelable
	{
	public:
		MenuBar() = default;
		~MenuBar() = default;
		MenuBar(const MenuBar& p_other) = default;
		MenuBar(MenuBar&& p_other) noexcept = default;

		MenuBar& operator=(const MenuBar&) = default;
		MenuBar& operator=(MenuBar&&) = default;

		void DisplayAndUpdatePanel() override;

		std::vector<Menu> m_menus;
	};
}