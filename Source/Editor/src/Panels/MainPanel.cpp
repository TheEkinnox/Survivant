//MainPanel.cpp

#include "SurvivantEditor/Panels/MainPanel.h"

#include <imgui.h>

namespace SvEditor::Panels
{
    MainPanel::MainPanel() :
        Panel(NAME),
        m_panelFlags(ERenderFlags())
    {}

    MainPanel::MainPanel(MenuBar&& p_menuBar) :
        Panel(NAME),
        m_panelFlags(ERenderFlags())
    {
        SetMenuBar(std::move(p_menuBar));
    }

    void MainPanel::RenderDockSpace()
    {
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
        static float borderSize = 5.0f;
        bool open = true;
        //fullscreen + undockable/cant move
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);        //push 2 styles
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, borderSize);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        //always begin bcs if not lose docked children
        ImGui::Begin("DockSpace Demo", &open, windowFlags);

        ImGui::PopStyleVar(2); //pop 2 styles

        // Submit the DockSpace
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

        ImGui::End();
    }

    Panel::ERenderFlags MainPanel::Render()
    {
        bool open = true;
        ImGui::Begin("DockSpace Demo", &open);

        if (!m_forceFocus.empty())
            SetForceFocust();

        if (m_layout)
            SetupLayout(static_cast<int>(ImGui::GetID("MyDockSpace")));

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

    void MainPanel::ForceFocus(const std::string& p_name)
    {
        m_forceFocus = p_name;
    }

    void MainPanel::SetupLayout(int p_dockspaceId)
    {
        m_layout(p_dockspaceId);
        m_layout = nullptr;
    }

    void MainPanel::SetForceFocust()
    {
        ImGui::SetWindowFocus(m_forceFocus.c_str());
        m_forceFocus.clear();
    }
}