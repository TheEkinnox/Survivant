//Menu.cpp

#include "SurvivantEditor/MenuItems/Menu.h"

#include <imgui.h>

//#include "imgui_internal.h"

namespace SvEditor::MenuItems
{
    Menu::Menu(const std::string& p_name)
        : m_name(p_name)
    {
    }

    Menu::Menu(const Menu& p_other)
    {
        *this = p_other;
    }

    Menu::Menu(Menu&& p_other)noexcept
    {
        this->m_name = p_other.m_name;
        this->m_items = std::move(p_other.m_items);
    }

    Menu& Menu::operator=(const Menu& p_other)
    {
        this->m_name = p_other.m_name;

        this->m_items.clear();
        this->m_items.reserve(p_other.m_items.size());
        for (const auto& item : p_other.m_items)
        {
            this->m_items.emplace_back();
            this->m_items.back().reset(item->Clone());
        }

        this->m_items.shrink_to_fit();

        return *this;
    }

    IMenuable* Menu::Clone() const
    {
        return new Menu(*this);
    }

    void Menu::DisplayAndUpdateMenu()
    {
        DisplayAndUpdate();
    }

    void Menu::DisplayAndUpdatePanel()
    {
        DisplayAndUpdate();
    }

    void Menu::SetName(const std::string& p_name)
    {
        m_name = p_name;
    }

    void Menu::DisplayAndUpdate()
    {
        if (ImGui::BeginMenu(m_name.c_str()))
        {
            for (auto& menu : m_items)
                menu->DisplayAndUpdateMenu();

            ImGui::EndMenu();
        }
    }
}