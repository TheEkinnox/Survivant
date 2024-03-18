//Menu.h
#pragma once

#include "SurvivantUI/Interfaces/IMenuable.h"
#include "SurvivantUI/Interfaces/IPanelable.h"

#include <string>
#include <vector>
#include <memory>

using namespace SvUI::Interfaces;

namespace SvUI::MenuItems
{
	class Menu : public IMenuable , public IPanelable
	{
	public:
		//unique name
		Menu(std::string p_name) : m_name(p_name) {};
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
