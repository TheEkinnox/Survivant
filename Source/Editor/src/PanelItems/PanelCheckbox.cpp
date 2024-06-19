#include "SurvivantEditor/PanelItems/PanelCheckbox.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelCheckbox::PanelCheckbox(const std::string& p_name, Value& p_value, const Callback& p_callback)
        : PanelCheckbox(p_name, GetRefFunc([p_value]() mutable -> Value& { return p_value; }), p_callback)
    {
    }

    PanelCheckbox::PanelCheckbox(std::string p_name, const GetRefFunc& p_refFunc, const Callback& p_callback)
        : PanelInputBase(p_refFunc, p_callback), m_name(std::move(p_name))
    {
    }

    PanelCheckbox::PanelCheckbox(std::string p_name, const GetCopyFunc& p_copyFunc, const Callback& p_callback)
        : PanelInputBase(p_copyFunc, p_callback), m_name(std::move(p_name))
    {
    }

    void PanelCheckbox::DisplayAndUpdatePanel()
    {
        Value& value = GetRef();
        ImGui::Text("%s", m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::Checkbox("##", &value) && m_callback)
            m_callback(value);

        ImGui::PopID();
    }
}
