//Menu.h
#pragma once

#include "SurvivantEditor/Interfaces/IMenuable.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <string>
#include <vector>
#include <memory>


namespace SvEditor::MenuItems
{
	using namespace SvEditor::Interfaces;

	class Menu : public IMenuable , public IPanelable
	{
	public:
		//unique name
		Menu(const std::string& p_name);
		~Menu() = default;
		Menu(const Menu& p_other);
		Menu(Menu&& p_other) noexcept;
		Menu& operator=(const Menu& p_other);

		/// <summary>
		/// Deep copy
		/// </summary>
		/// <returns></returns>
		IMenuable* Clone() const override;

		void DisplayAndUpdateMenu() override;
		void DisplayAndUpdatePanel() override;

		void SetName(const std::string& p_name);

		std::vector<std::unique_ptr<IMenuable>> m_items;

	private:
		void DisplayAndUpdate();

		std::string m_name;
	};

	
}
