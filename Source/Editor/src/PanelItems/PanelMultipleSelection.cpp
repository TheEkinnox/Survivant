//PanelMultipleSelection.cpp

#include "SurvivantEditor/PanelItems/PanelMultipleSelection.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelMultipleSelection::PanelMultipleSelection(
        const std::string& p_name,
        const std::vector<std::string>& p_selectable,
        const Callback& p_callback) :
        m_name(p_name),
        m_callback(p_callback)
    {
        m_curentSelection = 0;
        m_count = static_cast<int>(p_selectable.size());

        m_items = p_selectable;
    }

    void PanelMultipleSelection::DisplayAndUpdatePanel()
    {
        auto oldSelection = m_curentSelection;

        if (ImGui::BeginCombo(m_name.c_str(), m_displayString.c_str()))
        {
            for (int i = 0; i < m_items.size(); i++)
            {
                int flag = 1 << i;
                bool is_selected = (m_curentSelection & flag);

                if (ImGui::Selectable(m_items[i].c_str(), is_selected, ImGuiSelectableFlags_DontClosePopups))
                {
                    if (is_selected)
                        m_curentSelection &= ~flag;
                    else
                        m_curentSelection |= flag;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (oldSelection != m_curentSelection)
        {
            m_displayString = GetDisplayString();

            if (m_callback)
                m_callback(m_curentSelection);
        }
    }

    std::string PanelMultipleSelection::GetDisplayString()
    {
        static const char* Seperator = ", ";
        static const char* EmptyString = "(none)";
        std::string str;
        size_t count = 0;

        for (size_t i = 0; i < m_items.size(); i++)
        {
            int flag = 1 << i;
            if (m_curentSelection & flag)
            {
                if (count++ > 0)
                    str += Seperator;

                str += (m_items[i]);
            }
        }

        return str.empty() ? EmptyString : str;
    }
}