//MenuButton.cpp

#include "SurvivantUI/MenuItems/MenuButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


namespace SvUI::MenuItems
{
    using namespace SvUI::Interfaces;

    MenuButton::MenuButton(const std::string& p_name, const App::InputManager::KeyCallback& p_callback) :
        IMenuable()
    {
        m_name = p_name;
        m_callback = p_callback;
    }

    MenuButton::MenuButton(
        const std::string& p_name,
        const App::InputManager::KeyCallback& p_callback,
        const App::InputManager::KeyboardKeyType& p_shortcut) :
        MenuButton(p_name, p_callback)
    {
        AddShortcut(p_shortcut);
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

    void MenuButton::AddShortcut(const App::InputManager::KeyboardKeyType& p_shortcut)
    {
        auto& im = App::InputManager::GetInstance();
        im.AddInputBinding(p_shortcut, m_callback);
        m_shortcut = im.KeyBindingToString(p_shortcut);
    }
}