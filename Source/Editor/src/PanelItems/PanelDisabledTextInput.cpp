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
        ImGui::Text("%s", m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        ImGui::BeginDisabled();
        ImGui::InputText("##", m_string.data(), m_string.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();
        ImGui::PopID();
    }
}
