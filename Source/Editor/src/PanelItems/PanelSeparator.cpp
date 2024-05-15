#include "SurvivantEditor/PanelItems/PanelSeparator.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


namespace SvEditor::PanelItems
{
    void PanelSeparator::DisplayAndUpdatePanel()
    {
        ImGui::Separator();
    }
}
