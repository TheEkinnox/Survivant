//MainPanel.cpp

#include "SurvivantUI/Panels/MainPanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
{
    MainPanel::MainPanel() :
        Panel(NAME),
        m_panelFlags(ERenderFlags()),
        m_menuBar()
    {}

    MainPanel::MainPanel(MenuBar&& p_menuBar) :
        Panel(NAME),
        m_panelFlags(ERenderFlags()),
        m_menuBar()
    {
        SetMenuBar(std::move(p_menuBar));
    }

    Panel::ERenderFlags MainPanel::Render()
    {
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        bool open = true;
        //fullscreen + undockable/cant move
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        //always begin bcs if not lose docked childreen
        ImGui::Begin("DockSpace Demo", &open, windowFlags);

        //?
        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        if (m_layout)
            SetupLayout(static_cast<int>(dockspaceId));

        m_panelFlags = ERenderFlags();
        m_menuBar.DisplayAndUpdatePanel();
        ImGui::End();

        return m_panelFlags;
    }

    void MainPanel::SetMenuBar(MenuBar&& p_menuBar)
    {
        m_menuBar = std::move(p_menuBar);
    }

    void MainPanel::ChangePanelLayout(const ChangeLayout& p_layout)
    {
        m_layout = p_layout;
    }

    void MainPanel::SetupLayout(int p_dockspaceId)
    {
        m_layout(p_dockspaceId);
        m_layout = nullptr;
    }
}