//MenuBar.cpp

#include "SurvivantEditor/MenuItems/MenuBar.h"

#include <imgui.h>


namespace SvEditor::MenuItems
{
    void MenuBar::DisplayAndUpdatePanel()
    {
        if (ImGui::BeginMenuBar())
        {
            for (auto& menu : m_menus)
                menu.DisplayAndUpdateMenu();

            ImGui::EndMenuBar();
        }
    }
}