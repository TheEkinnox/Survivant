#include "SurvivantEditor/PanelItems/PanelDisabledTextInput.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelDisabledTextInput::PanelDisabledTextInput(std::string p_name, std::string p_string)
        : m_name(std::move(p_name)), m_string(std::move(p_string))
    {
    }

    void PanelDisabledTextInput::DisplayAndUpdatePanel()
    {
        ImGui::BeginDisabled();
        ImGui::InputText(m_name.c_str(), m_string.data(), m_string.size());
        ImGui::EndDisabled();
    }
}
