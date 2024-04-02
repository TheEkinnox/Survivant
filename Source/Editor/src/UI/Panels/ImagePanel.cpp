//ImagePanel.cpp

#include "SurvivantEditor/UI/Panels/ImagePanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::Panels
{
    Panel::ERenderFlags ImagePanel::Render()
    {
        static intptr_t tmp = 1;
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
        bool showWindow = true;

        ImGui::Begin(m_name.c_str(), &showWindow, window_flags);

        ImGui::Image((void*)m_textureId, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();


        ERenderFlags flags = ERenderFlags();
        if (!showWindow)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}