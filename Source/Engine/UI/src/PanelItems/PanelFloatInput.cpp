//PanelFloatInput.cpp

#include "SurvivantUI/PanelItems/PanelFloatInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    PanelFloatInput::PanelFloatInput(
        const std::string& p_name,
        float& p_value,
        const Callback& p_callback) :
        m_name(p_name),
        m_value(p_value),
        m_callback(p_callback)
    {}

    void PanelFloatInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        if (ImGui::InputFloat(m_name.c_str(), &m_value, 0, 0, "%.3f", flags))
            m_callback(m_value);
    }
}