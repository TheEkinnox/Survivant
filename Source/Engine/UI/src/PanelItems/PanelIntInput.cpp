//PanelIntInput.cpp

#include "SurvivantUI/PanelItems/PanelIntInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


namespace SvUI::PanelItems
{
    PanelIntInput::PanelIntInput(
        const std::string& p_name,
        int& p_value,
        const Callback& p_callback) :
        m_name(p_name),
        m_value(p_value),
        m_callback(p_callback)
    {}

    void PanelIntInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        if (ImGui::InputInt(m_name.c_str(), &m_value, 0, 0, flags))
            m_callback(m_value);
    }
}