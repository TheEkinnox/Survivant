//MenuBar.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/MenuItems/Menu.h"

#include <vector>

using namespace SvEditor::Interfaces;

namespace SvEditor::MenuItems
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