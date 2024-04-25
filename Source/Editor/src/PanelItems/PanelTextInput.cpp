//PanelTextInput.cpp

#include "SurvivantEditor/PanelItems/PanelTextInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "imgui_internal.h"

struct InputTextCallback_UserData
{
    std::string* Str;
    ImGuiInputTextCallback  ChainCallback;
    void* ChainCallbackUserData;
};

//code from imgui_stdlib.cpp
int InputTextCallback(::ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}


//code from imgui_stdlib.cpp
bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;

    using namespace std::placeholders;

    return ImGui::InputText(
        label, (char*)str->c_str(), str->capacity() + 1, flags,
        InputTextCallback,
        &cb_user_data);
}

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
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
            ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

        bool reclaim_focus = false;

        auto& buffer = GetRef();


        ImGui::Text(m_name.c_str());
        ImGui::SameLine();

        ImGui::PushID(m_name.c_str());
        if (InputText("##", &buffer, input_text_flags, nullptr, (void*)this))
        {
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

    void PanelTextInput::ClearPanelText(PanelTextInput& p_panel)
    {
        p_panel.Clear();
    }
}

