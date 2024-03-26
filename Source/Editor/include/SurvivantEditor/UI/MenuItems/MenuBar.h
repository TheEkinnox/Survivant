//MenuBar.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/MenuItems/Menu.h"

#include <vector>

using namespace SvEditor::UI::Interfaces;

namespace SvEditor::UI::MenuItems
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