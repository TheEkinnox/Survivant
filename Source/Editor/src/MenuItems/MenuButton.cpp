//MenuButton.cpp

#include "SurvivantEditor/MenuItems/MenuButton.h"

#include <imgui.h>


namespace SvEditor::MenuItems
{
    using namespace SvEditor::Interfaces;

    MenuButton::MenuButton(const std::string& p_name, const SvApp::InputManager::KeyCallback& p_callback) :
        IMenuable()
    {
        m_name = p_name;
        m_callback = p_callback;
    }

    MenuButton::MenuButton(
        const std::string& p_name,
        const SvApp::InputManager::KeyCallback& p_callback,
        const SvApp::InputManager::KeyboardKeyType& p_shortcut,
        SvApp::InputManager::InputBindings& p_inputs) :
        MenuButton(p_name, p_callback)
    {
        AddShortcut(p_inputs, p_shortcut);
    }

    IMenuable* MenuButton::Clone() const
    {
        return new MenuButton(*this);
    }

    void MenuButton::DisplayAndUpdateMenu()
    {
        if (ImGui::MenuItem(m_name.c_str(), m_shortcut.c_str(), false, true))
            m_callback(0);
    }

    void MenuButton::AddShortcut(SvApp::InputManager::InputBindings& p_inputs, const SvApp::InputManager::KeyboardKeyType& p_shortcut)
    {
        auto& im = SvApp::InputManager::GetInstance();
        p_inputs.m_keyCallbacks.emplace(p_shortcut, m_callback);
        m_shortcut = im.KeyBindingToString(p_shortcut);
    }
}