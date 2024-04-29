//PanelFloatInput.cpp

#include "SurvivantEditor/PanelItems/PanelFloatInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelFloatInput::PanelFloatInput(
        const std::string& p_name,
        float& p_value,
        const Callback& p_callback) :
        PanelFloatInput(p_name, GetRefFunc([p_value]() mutable -> float& { return p_value; }), p_callback)
    {}

    PanelFloatInput::PanelFloatInput(
        const std::string & p_name, const GetRefFunc & p_getRef, const Callback & p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {}

    PanelFloatInput::PanelFloatInput(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name)
    {}

    void PanelFloatInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto value = GetRef();
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::InputFloat("##", &value, 0, 0, "%.3f", flags) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }
}