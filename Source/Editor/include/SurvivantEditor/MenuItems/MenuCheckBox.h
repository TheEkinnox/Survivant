//MenuCheckBox.h
#pragma once

#include "SurvivantEditor/Interfaces/IMenuable.h"

#include <string>

namespace SvEditor::MenuItems
{
	class MenuCheckBox : public Interfaces::IMenuable
	{
	public:
		MenuCheckBox(const std::string& p_name, bool& p_isChecked);
		~MenuCheckBox() = default;

		IMenuable* Clone() const override;
		void DisplayAndUpdateMenu() override;

	private:
		std::string		m_name;
		bool*			m_isCheked;
	};
}