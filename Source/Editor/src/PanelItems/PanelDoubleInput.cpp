#include "SurvivantEditor/PanelItems/PanelDoubleInput.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


namespace SvEditor::PanelItems
{
    PanelDoubleInput::PanelDoubleInput(const std::string& p_name, Value& p_value, const Callback& p_callback)
        : PanelDoubleInput(p_name, GetRefFunc([p_value]() mutable -> Value& { return p_value; }), p_callback)
    {
    }

    PanelDoubleInput::PanelDoubleInput(std::string p_name, const GetRefFunc& p_refFunc, const Callback& p_callback)
        : PanelInputBase(p_refFunc, p_callback), m_name(std::move(p_name))
    {
    }

    PanelDoubleInput::PanelDoubleInput(std::string p_name, const GetCopyFunc& p_copyFunc, const Callback& p_callback)
        : PanelInputBase(p_copyFunc, p_callback), m_name(std::move(p_name))
    {
    }

    void PanelDoubleInput::DisplayAndUpdatePanel()
    {
        static constexpr int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        Value& value = GetRef();
        ImGui::Text("%s", m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::InputDouble("##", &value, 0, 0, "%.3f", flags) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }
}
