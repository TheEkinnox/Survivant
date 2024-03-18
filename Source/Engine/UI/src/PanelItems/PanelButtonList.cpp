//PanelButtonList.cpp

#include "SurvivantUI/PanelItems/PanelButtonList.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    void PanelButtonList::DisplayAndUpdatePanel()
    {
        if (m_buttons.empty())
            return;

        auto buttonItt = m_buttons.begin();
        buttonItt->DisplayAndUpdatePanel();
        buttonItt++;

        for (auto& itt = buttonItt; itt != m_buttons.end(); itt++)
        {
            ImGui::SameLine();
            itt->DisplayAndUpdatePanel();
        }
    }
}