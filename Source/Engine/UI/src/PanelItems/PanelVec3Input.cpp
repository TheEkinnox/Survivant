//PanelVec3Input.cpp

#include "SurvivantUI/PanelItems/PanelVec3Input.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    PanelVec3Input::PanelVec3Input(
        const std::string& p_name,
        LibMath::Vector3& p_value,
        const Callback& p_callback) :
        m_name(p_name),
        m_value(p_value),
        m_callback(p_callback)
    {}

    void PanelVec3Input::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        if (ImGui::InputFloat3(m_name.c_str(), m_value.getArray(), "%3.f", flags))
            m_callback(m_value);
    }


}