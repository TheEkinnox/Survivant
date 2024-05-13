//PopupMenu.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Interfaces/IMenuable.h"

#include <vector>
#include <memory>

namespace SvEditor::MenuItems
{
	using namespace SvEditor::Interfaces;
	
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