//PopupMenu.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/Interfaces/IMenuable.h"

#include <vector>
#include <memory>

namespace SvUI::MenuItems
{
	using namespace SvUI::Interfaces;
	
	class PopupMenu : public IPanelable
	{
	public:
		PopupMenu() : m_isOpen(false) {}
		~PopupMenu() = default;
		PopupMenu(const PopupMenu& p_other);
		PopupMenu(PopupMenu&& p_other) noexcept;
		PopupMenu& operator=(const PopupMenu& p_other);

		PopupMenu* Clone() const;

		//IPanelable
		void DisplayAndUpdatePanel() override;

		bool IsEmpty() { return m_items.size() == 0; }

		std::vector<std::unique_ptr<IMenuable>> m_items;

		bool m_isOpen;
	};
}
