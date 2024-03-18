//ImagePanel.cpp

#include "SurvivantUI/Panels/ImagePanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
{
    Panel::ERenderFlags ImagePanel::Render()
    {
        static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
        bool showWindow = true;

        ImGui::Begin(m_name.c_str(), &showWindow, window_flags);

        ImGui::Image((void*)m_textureId, ImGui::GetWindowSize());
        ImGui::End();


        ERenderFlags flags = ERenderFlags();
        if (!showWindow)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}