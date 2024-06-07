//SavePanel.cpp
#include "SurvivantEditor/Panels/SavePanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantApp/Windows/Window.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <stdexcept>


namespace SvEditor::Panels
{
    SavePanel::SavePanel()
    {
        m_name = NAME;

        m_options.m_buttons.push_back(PanelButton("Save",
            []() 
            { 
                SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnSave>();

                if (Core::EditorEngine::OnSave::s_saveSucceded)
                    SvCore::Events::EventManager::GetInstance().Invoke<SvApp::Window::WindowClosing>(); 
            }));
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
        static const ImGuiWindowFlags f = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImGui::OpenPopup(m_name.c_str());

        if (ImGui::BeginPopupModal(m_name.c_str(), &m_open, f))
        {
            m_options.DisplayAndUpdatePanel();
            ImGui::EndPopup();
        }

        ERenderFlags flags = ERenderFlags();
        if (!m_open)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}