//SavePanel.cpp

#include "SurvivantEditor/UI/Panels/SavePanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantApp/Windows/Window.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::UI::Panels
{
    SavePanel::SavePanel()
    {
        m_name = NAME;

        m_options.m_buttons.push_back(PanelButton("Save",
            []() { SvCore::Events::EventManager::GetInstance().Invoke<SvApp::Window::WindowClosing>(); }));
        m_options.m_buttons.push_back(PanelButton("Don't Save",
            []() { SvCore::Events::EventManager::GetInstance().Invoke<SvApp::Window::WindowClosing>(); }));
        m_options.m_buttons.push_back(PanelButton("Close",
            [this]() { m_open = false; }));
    }

    SavePanel::~SavePanel()
    {
    }

    Panel::ERenderFlags SavePanel::Render()
    {
        if (ImGui::Begin(m_name.c_str(), &m_open))
        {
            //buttons or 'x' can close
            m_options.DisplayAndUpdatePanel();
        }


        ImGui::End();

        ERenderFlags flags = ERenderFlags();
        if (!m_open)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}