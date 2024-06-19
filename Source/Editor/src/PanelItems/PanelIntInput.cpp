//PanelIntInput.cpp

#include "SurvivantEditor/PanelItems/PanelIntInput.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelIntInput::PanelIntInput(const std::string& p_name, int& p_value, const Callback& p_callback) :
        PanelIntInput(p_name, GetRefFunc([p_value]() mutable -> int& { return p_value; }), p_callback)
    {}

    PanelIntInput::PanelIntInput(const std::string& p_name, const GetRefFunc& p_refFunc, const Callback& p_callback) :
        PanelInputBase(p_refFunc, p_callback),
        m_name(p_name)
    {}

    PanelIntInput::PanelIntInput(const std::string& p_name, const GetCopyFunc& p_copyFunc, const Callback& p_callback) :
        PanelInputBase(p_copyFunc, p_callback),
        m_name(p_name)
    {}

    void PanelIntInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto& value = GetRef();
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        PanelInputBase::DisplayAndUpdatePanel();
        if (ImGui::InputInt("##", &value, 0, 0, flags) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }
}