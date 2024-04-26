//PanelVec2Input.cpp

#include "SurvivantEditor/PanelItems/PanelVec2Input.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelVec2Input::PanelVec2Input(
        const std::string& p_name,
        LibMath::Vector2& p_value,
        const Callback& p_callback) :
        PanelVec2Input(p_name, GetRefFunc([p_value]() mutable -> LibMath::Vector2& { return p_value; }), p_callback)
    {}

    PanelVec2Input::PanelVec2Input(
        const std::string & p_name, const GetRefFunc & p_getRef, const Callback & p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {}

    PanelVec2Input::PanelVec2Input(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),

        m_name(p_name)
    {}

    void PanelVec2Input::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto& value = GetRef();
        if (ImGui::InputFloat2(m_name.c_str(), value.getArray(), "%3.f", flags) && m_callback)
            m_callback(value);
    }


}