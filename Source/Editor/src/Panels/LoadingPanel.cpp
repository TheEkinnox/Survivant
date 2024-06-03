//SavePanel.cpp
#include "SurvivantEditor/Panels/LoadingPanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantApp/Windows/Window.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::Panels
{
    LoadingPanel::LoadingPanel() :
        Panel(NAME)
    {}

    LoadingPanel::ERenderFlags LoadingPanel::Render()
	{
        static ImGuiWindowFlags flags =
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs;

        if (!m_open)
            return ERenderFlags::CLOSE;
        
        ImGui::SetNextWindowBgAlpha(0.3f);
        ImGui::Begin("##", nullptr, flags);
        ImGui::End();

        m_open = true;

		return ERenderFlags();
	}
}