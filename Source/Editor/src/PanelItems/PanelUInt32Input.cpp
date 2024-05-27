//PanelUInt32Input.cpp

#include "SurvivantEditor/PanelItems/PanelUInt32Input.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelUInt32Input::PanelUInt32Input(
        const std::string& p_name,
        Value& p_value,
        const Callback& p_callback) :
        PanelUInt32Input(p_name, GetRefFunc([p_value]() mutable -> Value& { return p_value; }), p_callback)
    {}

    PanelUInt32Input::PanelUInt32Input(
        const std::string & p_name, const GetRefFunc & p_getRef, const Callback & p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {}

    PanelUInt32Input::PanelUInt32Input(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),

        m_name(p_name)
    {}

    void PanelUInt32Input::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto& value = GetRef();
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        PanelInputBase::DisplayAndUpdatePanel();
        if (ImGui::InputScalar("##", ImGuiDataType_U32, &value, 0, 0, "%u", flags) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }
}