//PanelUniqueSelection.cpp

#include "SurvivantEditor/UI/PanelItems/PanelUniqueSelection.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::PanelItems
{
    PanelUniqueSelection::PanelUniqueSelection(
        const std::string& p_name,
        const std::vector<std::string>& p_selectable,
        const Callback& p_callback) :
        m_name(p_name),
        m_callback(p_callback)
    {
        m_curentSelection = 0;
        m_count = static_cast<int>(p_selectable.size());

        for (auto& item : p_selectable)
            m_items += (item + '\0');
    }

    void PanelUniqueSelection::DisplayAndUpdatePanel()
    {
        if (ImGui::Combo(m_name.c_str(), &m_curentSelection, m_items.c_str()) && m_callback != nullptr)
            m_callback(m_curentSelection);
    }
}
