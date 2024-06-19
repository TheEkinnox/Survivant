//SavePanel.cpp
#include "SurvivantEditor/Panels/LoadingPanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantApp/Windows/Window.h>

#include <imgui.h>

#include <stdexcept>


namespace SvEditor::Panels
{
    LoadingPanel::LoadingPanel() :
        Panel(NAME)
    {}

    LoadingPanel::ERenderFlags LoadingPanel::Render()
	{
        static const float alpha = 0.4f;
        static const ImGuiWindowFlags flags =
            //ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs;

        if (!m_open)
            return ERenderFlags::CLOSE;


        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowBgAlpha(alpha);

        ImGui::Begin("##", nullptr, flags);
        ImGui::End();

        //ImGui::OpenPopup("Loading...");

        //if(ImGui::BeginPopupModal("Loading...", nullptr, flags))
        //{

        //    ImGui::EndPopup();
        //}



        m_open = false;

		return ERenderFlags();
	}
}