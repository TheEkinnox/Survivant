//TestPanel.cpp

#include "SurvivantEditor/Panels/TestPanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::Panels
{
    TestPanel::TestPanel(const std::string& p_name) :
        Panel(p_name)
        //m_unique("UNIQUE", { "Opt 1", "Opt 2", "Opt 3" }, nullptr),
        //m_multiple("MULTIPLE", { "Opt 1", "Opt 2", "Opt 3" }, nullptr)
    {}

    Panel::ERenderFlags TestPanel::Render()
    {
        bool showWindow = true;

        ImGui::Begin(m_name.c_str(), &showWindow);
        ImGui::Text("TESSSSSSSSSSSSTTTTTTTTTT");
        if (ImGui::Button("Close Me"))
            showWindow = false;


        //m_unique.DisplayAndUpdatePanel();
        //m_multiple.DisplayAndUpdatePanel();

        ImGui::End();

        ERenderFlags flags = ERenderFlags();
        if (!showWindow)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}