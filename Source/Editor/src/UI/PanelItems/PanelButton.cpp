//PanelButton.cpp

#include "SurvivantEditor/UI/PanelItems/PanelButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::PanelItems
{
    void PanelButton::DisplayAndUpdatePanel()
    {
        if (ImGui::SmallButton(m_name.c_str()))
            m_callback.Invoke();
    }
}