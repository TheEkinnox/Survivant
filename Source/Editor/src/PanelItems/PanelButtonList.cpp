//PanelButtonList.cpp
#include "SurvivantEditor/PanelItems/PanelButtonList.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    void PanelButtonList::DisplayAndUpdatePanel()
    {
        for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
        {
            if (it != m_buttons.begin())
                ImGui::SameLine();

            it->DisplayAndUpdatePanel();
        }
    }
}
