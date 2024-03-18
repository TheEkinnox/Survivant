//MenuBar.cpp

#include "SurvivantUI/MenuItems/MenuBar.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


namespace SvUI::MenuItems
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