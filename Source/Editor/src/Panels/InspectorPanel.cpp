//InspectorPanel.cpp
#include "SurvivantEditor/Panels/InspectorPanel.h"

#include "SurvivantEditor/Core/IUI.h"

#include "SurvivantEditor/Panels/ExamplGameObj.h"
#include "SurvivantEditor/Panels/Panel.h"

#include <imgui.h>

namespace SvEditor::Panels
{
    InspectorPanel::InspectorPanel()
    {
        m_name = NAME;
    }

    InspectorPanel::~InspectorPanel()
    {
    }

    void InspectorPanel::SetInpectorInfo(
        const std::shared_ptr<IInspectorable>& p_selected, const std::string& p_resourceName)
    {
        s_selected = p_selected;
        s_resourceName = "(" + p_resourceName + ')';
    }

    void InspectorPanel::ClearInfo()
    {
        s_selected.reset();
    }

    Panel::ERenderFlags InspectorPanel::Render()
    {
        static bool open = true;
        static auto font = SV_CURRENT_UI()->GetFontDefault();
        static ImGuiWindowFlags    window_flags = ImGuiWindowFlags_None;
        static ImGuiChildFlags     child_flags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;

        if (ImGui::Begin(m_name.c_str(), &open) && s_selected)
        {
            auto cursorPos = ImGui::GetCursorPos();

            ImGui::PushFont(font);
            ImGui::Text("%s", s_selected->GetIcon().c_str());
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::Text(s_selected->GetName().c_str());

            ImGui::SameLine();
            ImGui::Text(s_resourceName.c_str());

            ImGui::Separator();

            //if (ImGui::BeginChild("Components", ImGui::GetContentR  egionAvail()), child_flags, window_flags)
            s_selected->DisplayAndUpdatePanel();

            //ImGui::EndChild();
        }



        ImGui::End();

        auto flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags::CLOSE;

        return flags;
    }
}