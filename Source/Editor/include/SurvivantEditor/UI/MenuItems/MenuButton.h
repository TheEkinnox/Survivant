//MenuButton.h
#pragma once

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantEditor/UI/Interfaces/IMenuable.h"

#include <memory>

namespace SvEditor::UI::MenuItems
{
	class MenuButton : public Interfaces::IMenuable
	{
	public:
		using KeyPair = std::pair<SvApp::InputManager::KeyboardKeyType, SvApp::InputManager::KeyCallback>;

		MenuButton(const std::string& p_name, const SvApp::InputManager::KeyCallback& p_callback);
		MenuButton(
			const std::string& p_name,
			const SvApp::InputManager::KeyCallback& p_callback,
			const SvApp::InputManager::KeyboardKeyType& p_shortcut, 
			SvApp::InputManager::InputBindings& p_inputs);
		~MenuButton()override = default;

		IMenuable* Clone() const override;

		void DisplayAndUpdateMenu() override;

		void AddShortcut(SvApp::InputManager::InputBindings& p_inputs, const SvApp::InputManager::KeyboardKeyType& p_shortcut);

	private:
		std::string							m_name;
		std::string							m_shortcut;
		SvApp::InputManager::KeyCallback	m_callback;
	};
}