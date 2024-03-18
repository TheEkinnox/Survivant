//MenuButton.h
#pragma once

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantUI/Interfaces/IMenuable.h"

#include <memory>

namespace SvUI::MenuItems
{
	class MenuButton : public Interfaces::IMenuable
	{
	public:
		using KeyPair = std::pair<App::InputManager::KeyboardKeyType, App::InputManager::KeyCallback>;

		MenuButton(const std::string& p_name, const App::InputManager::KeyCallback& p_callback);
		MenuButton(
			const std::string& p_name,
			const App::InputManager::KeyCallback& p_callback,
			const App::InputManager::KeyboardKeyType& p_shortcut);
		~MenuButton()override = default;

		IMenuable* Clone() const override;

		void DisplayAndUpdateMenu() override;

		void AddShortcut(const App::InputManager::KeyboardKeyType& p_shortcut);

	private:
		std::string						m_name;
		std::string						m_shortcut;
		App::InputManager::KeyCallback	m_callback;
	};
}