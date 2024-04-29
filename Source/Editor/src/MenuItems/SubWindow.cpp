////SubWindow.cpp
//C:\Users\vbold\Documents\GitHub\isart_projects_2\Survivant\Source\Editor\src\MenuItems\SubWindow.cpp
//#include "SurvivantEditor/SubWindow.h"
//
//#include "SurvivantApp/Windows/Window.h"
//#include "SurvivantCore/Events/EventManager.h"
//#include "SurvivantEditor/Panel.h"
//#include "SurvivantEditor/UI.h"
//
//#include "imgui.h"
//#include "backends/imgui_impl_glfw.h"
//#include "backends/imgui_impl_opengl3.h"
//
//#include <algorithm>
//
//
//using namespace UI;
//
//
//
//void tmpCallback(PanelTreeBranch& /*p_branch*/)
//{
//    std::string str = "Tree callback works";
//    //p_branch.ForceCloseChildreen();
//    SvCore::Events::EventManager::GetInstance().Invoke<UI::EditorUI::DebugEvent>(str.c_str());
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//UI::ConsolePanel::LogText::LogText(const LogInfo& p_logInfo) :
//    m_logInfo(p_logInfo)
//{
//    m_length = GetString().length();
//}
//
//void UI::ConsolePanel::LogText::DisplayAndUpdatePanel()
//{
//    ImVec4 color;
//    LogTypeColor(m_logInfo.m_type, &color);
//    bool has_color = &color != nullptr;
//
//    if (has_color)
//        ImGui::PushStyleColor(ImGuiCol_Text, color);
//
//    ImGui::TextUnformatted(LogTypeToString(m_logInfo.m_type).c_str());
//
//    if (has_color)
//        ImGui::PopStyleColor();
//
//    ImGui::SameLine();
//    ImGui::TextUnformatted((SPACER + m_logInfo.m_message).c_str());
//}
//
//std::string UI::ConsolePanel::LogText::GetString(size_t p_len)const
//{
//    if (p_len == 0)
//        return  std::string((LogTypeToString(m_logInfo.m_type) + SPACER + m_logInfo.m_message));
//
//    auto tmp = std::string((LogTypeToString(m_logInfo.m_type) + SPACER + m_logInfo.m_message), 0, p_len);
//    return tmp;
//}
//
//size_t UI::ConsolePanel::LogText::GetLength()const
//{
//    return m_length;
//
//}
//
//
//struct InputTextCallback_UserData
//{
//    std::string* Str;
//    ImGuiInputTextCallback  ChainCallback;
//    void* ChainCallbackUserData;
//};
//
////code from imgui_stdlib.cpp
//int InputTextCallback(::ImGuiInputTextCallbackData* data)
//{
//    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
//    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
//    {
//        // Resize string callback
//        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
//        std::string* str = user_data->Str;
//        IM_ASSERT(data->Buf == str->c_str());
//        str->resize(data->BufTextLen);
//        data->Buf = (char*)str->c_str();
//    }
//    else if (user_data->ChainCallback)
//    {
//        // Forward to user callback, if any
//        data->UserData = user_data->ChainCallbackUserData;
//        return user_data->ChainCallback(data);
//    }
//    return 0;
//}
//
////code from imgui_stdlib.cpp
//bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
//{
//    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
//    flags |= ImGuiInputTextFlags_CallbackResize;
//
//    InputTextCallback_UserData cb_user_data;
//    cb_user_data.Str = str;
//    cb_user_data.ChainCallback = callback;
//    cb_user_data.ChainCallbackUserData = user_data;
//
//    using namespace std::placeholders;
//
//    return ImGui::InputText(
//        label, (char*)str->c_str(), str->capacity() + 1, flags,
//        InputTextCallback,
//        &cb_user_data);
//}
//
//
//
//
////ImFont& GetBigFont()
////{
////    static float FONT_SIZE = 100;
////
////    ImFont myFont = *ImGui::GetFont();
////    myFont.FontSize = FONT_SIZE;
////
////    return myFont;
////}
//
//
//
//
//
//
//
//
//
