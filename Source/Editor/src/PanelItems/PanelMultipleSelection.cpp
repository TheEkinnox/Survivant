//PanelMultipleSelection.cpp

#include "SurvivantEditor/PanelItems/PanelMultipleSelection.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelMultipleSelection::PanelMultipleSelection(
        const std::string& p_name, const std::vector<std::string>& p_selectable, 
        int& p_currentSelection, const Callback& p_callback) :
        PanelMultipleSelection(p_name, p_selectable,
            GetRefFunc([p_currentSelection]() mutable -> int& { return p_currentSelection; }), p_callback)
    {}

    PanelMultipleSelection::PanelMultipleSelection(
        const std::string& p_name, const std::vector<std::string>& p_selectable, 
        const GetRefFunc& p_getRef, const Callback& p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name),
        m_count(static_cast<int>(p_selectable.size())),
        m_items(p_selectable)
    {
        m_displayString = GetDisplayString(GetRef());
    }

    PanelMultipleSelection::PanelMultipleSelection(
        const std::string& p_name, const std::vector<std::string>& p_selectable, 
        const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name),
        m_count(static_cast<int>(p_selectable.size())),
        m_items(p_selectable)
    {
        m_displayString = GetDisplayString(GetRef());
    }

    void PanelMultipleSelection::DisplayAndUpdatePanel()
    {
        int& newSelection = GetRef();
        int oldSelection = newSelection;

        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::BeginCombo("##", m_displayString.c_str()))
        {
            for (int i = 0; i < m_items.size(); i++)
            {
                int flag = 1 << i;
                bool is_selected = (newSelection & flag);

                if (ImGui::Selectable(m_items[i].c_str(), is_selected, ImGuiSelectableFlags_DontClosePopups))
                {
                    if (is_selected)
                        newSelection &= ~flag;
                    else
                        newSelection |= flag;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopID();

        if (oldSelection != newSelection)
        {
            m_displayString = GetDisplayString(newSelection);

            if (m_callback)
                m_callback(newSelection);
        }
    }

    std::string PanelMultipleSelection::GetDisplayString(int p_selection)
    {
        static const char* Seperator = ", ";
        static const char* EmptyString = "(none)";
        std::string str;
        size_t count = 0;

        for (size_t i = 0; i < m_items.size(); i++)
        {
            int flag = 1 << i;
            if (p_selection & flag)
            {
                if (count++ > 0)
                    str += Seperator;

                str += (m_items[i]);
            }
        }

        return str.empty() ? EmptyString : str;
    }
}