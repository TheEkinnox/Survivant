//PanelButton.cpp

#include "SurvivantEditor/PanelItems/PanelButton.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    void PanelButton::DisplayAndUpdatePanel()
    {
        if (ImGui::SmallButton(m_name.c_str()))
            m_callback.Invoke();
    }
}