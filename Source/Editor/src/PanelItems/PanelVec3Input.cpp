//PanelVec3Input.cpp

#include "SurvivantEditor/PanelItems/PanelVec3Input.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelVec3Input::PanelVec3Input(
        const std::string& p_name,
        LibMath::Vector3& p_value,
        const Callback& p_callback,
        float p_min,
        float p_max) :
        PanelVec3Input(p_name, GetRefFunc([p_value]() mutable -> LibMath::Vector3& { return p_value; }), 
            p_callback, p_min, p_max)
    {}

    PanelVec3Input::PanelVec3Input(
        const std::string & p_name, const GetRefFunc & p_getRef, const Callback & p_callback,
        float p_min,
        float p_max) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name),
        m_min(p_min),
        m_max(p_max)
    {}

    PanelVec3Input::PanelVec3Input(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback,
        float p_min,
        float p_max) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name),
        m_min(p_min),
        m_max(p_max)
    {}

    void PanelVec3Input::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto& value = GetRef();
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::InputFloat3("##", value.getArray(), "%0.3f", flags))
        {
            value.m_x = LibMath::clamp(value.m_x, m_min, m_max);
            value.m_y = LibMath::clamp(value.m_y, m_min, m_max);
            value.m_z = LibMath::clamp(value.m_z, m_min, m_max);

            if (m_callback)
                m_callback(value);
        }

        ImGui::PopID();
    }
}