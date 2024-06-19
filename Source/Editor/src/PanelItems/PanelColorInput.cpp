//PanelVec3Input.cpp

#include "SurvivantEditor/PanelItems/PanelColorInput.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelColorInput::PanelColorInput(
        const std::string& p_name,
        LibMath::Vector4& p_value,
        const Callback& p_callback) :
        PanelColorInput(p_name, GetRefFunc([p_value]() mutable -> LibMath::Vector4& { return p_value; }), p_callback)
    {}

    PanelColorInput::PanelColorInput(
        const std::string & p_name, const GetRefFunc & p_getRef, const Callback & p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {}

    PanelColorInput::PanelColorInput(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name)
    {}

    void PanelColorInput::DisplayAndUpdatePanel()
    {
        //ImGuiColorEditFlags_
        auto& value = GetRef();

        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        PanelInputBase::DisplayAndUpdatePanel();
        if (ImGui::ColorEdit4("##", value.getArray()) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }


}