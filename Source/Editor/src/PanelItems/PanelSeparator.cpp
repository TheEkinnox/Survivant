#include "SurvivantEditor/PanelItems/PanelSeparator.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelSeparator::PanelSeparator(std::string p_text)
        : m_text(std::move(p_text))
    {
    }

    void PanelSeparator::DisplayAndUpdatePanel()
    {
        if (m_text.empty())
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
        else
        {
            ImGui::SeparatorText(m_text.c_str());
        }
    }
}
