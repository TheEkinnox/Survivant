//SavePanel.cpp

#include "SurvivantUI/Panels/SavePanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantApp/Windows/Window.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvUI::Panels
{
    SavePanel::SavePanel()
    {
        m_name = GetUniqueName(NAME, s_panelCount);
        s_panelCount++;

        m_options.m_buttons.push_back(PanelButton("Save",
            []() { Core::EventManager::GetInstance().Invoke<App::Window::WindowClosing>(); }));
        m_options.m_buttons.push_back(PanelButton("Don't Save",
            []() { Core::EventManager::GetInstance().Invoke<App::Window::WindowClosing>(); }));
        m_options.m_buttons.push_back(PanelButton("Close",
            [this]() { m_open = false; }));
    }

    SavePanel::~SavePanel()
    {
        s_panelCount--;
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