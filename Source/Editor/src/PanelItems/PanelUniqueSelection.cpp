//PanelUniqueSelection.cpp

#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelUniqueSelection::PanelUniqueSelection(
        const std::string& p_name,
        const std::vector<std::string>& p_selectable,
        int& p_currentSelection,
        const Callback& p_callback) :
        PanelUniqueSelection(p_name, p_selectable,
            GetRefFunc([p_currentSelection]() mutable -> int& { return p_currentSelection; }), p_callback)
    {}

    PanelUniqueSelection::PanelUniqueSelection(
        const std::string& p_name, const std::vector<std::string>& p_selectable,
        const GetRefFunc& p_getRef, const Callback& p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {
        m_count = static_cast<int>(p_selectable.size());

        for (auto& item : p_selectable)
            m_items += (item + '\0');
    }

    PanelUniqueSelection::PanelUniqueSelection(
        const std::string& p_name, const std::vector<std::string>& p_selectable,
        const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name)
    {
        m_count = static_cast<int>(p_selectable.size());

        for (auto& item : p_selectable)
            m_items += (item + '\0');
    }

    void PanelUniqueSelection::DisplayAndUpdatePanel()
    {
        auto& val = GetRef();
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::Combo("##", &val, m_items.c_str()) && m_callback)
            m_callback(val);

        ImGui::PopID();
    }
}
