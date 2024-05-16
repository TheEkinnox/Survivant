//PanelTextInput.cpp

#include "SurvivantEditor/PanelItems/PanelTextInput.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace SvEditor::PanelItems
{
    PanelTextInput::PanelTextInput(
        const std::string& p_name,
        std::string& p_buffer,
        const Callback& p_callback) :
        PanelTextInput(p_name, GetRefFunc([p_buffer]() mutable -> std::string& { return p_buffer; }), p_callback)
    {}

    PanelTextInput::PanelTextInput(
        const std::string& p_name, const GetRefFunc& p_getRef, const Callback& p_callback) :
        PanelInputBase(p_getRef, p_callback),
        m_name(p_name)
    {}

    PanelTextInput::PanelTextInput(
        const std::string& p_name, const GetCopyFunc& p_getCopy, const Callback& p_callback) :
        PanelInputBase(p_getCopy, p_callback),
        m_name(p_name)
    {
        m_callback = p_callback;
    }

    void PanelTextInput::DisplayAndUpdatePanel()
    {
        //Command-line
        static ImGuiInputTextFlags input_text_flags =
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

        bool reclaim_focus = false;

        auto& buffer = GetRef();


        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (ImGui::InputText("##", &buffer, input_text_flags, nullptr, this))
        {
            m_string = buffer;

            //when iconTxt is finished being inputed
            if (m_callback)
                m_callback({ this });

            reclaim_focus = true;
        }
        ImGui::PopID();


        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    void PanelTextInput::Clear()
    {
        GetRef().clear();
    }

    const std::string& PanelTextInput::GetText() const
    {
        return GetRef();
    }

    const std::string& PanelTextInput::GetUpdatedText() const
    {
        return m_string;
    }

    void PanelTextInput::ClearPanelText(PanelTextInput& p_panel)
    {
        p_panel.Clear();
    }
}

