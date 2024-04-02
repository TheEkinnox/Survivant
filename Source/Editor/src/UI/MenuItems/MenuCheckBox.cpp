//MenuCheckBox.cpp

#include "SurvivantEditor/UI/MenuItems/MenuCheckBox.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::MenuItems
{
    using namespace SvEditor::UI::Interfaces;

    MenuCheckBox::MenuCheckBox(const std::string& p_name, bool& p_isChecked) :
        m_name(p_name),
        m_isCheked(&p_isChecked)
    {}

    IMenuable* MenuCheckBox::Clone() const
    {
        return new MenuCheckBox(*this);
    }

    void MenuCheckBox::DisplayAndUpdateMenu()
    {
        if (ImGui::MenuItem(m_name.c_str(), nullptr, *m_isCheked, true))
            *m_isCheked = !(*m_isCheked);
    }
}