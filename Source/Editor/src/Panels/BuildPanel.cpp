//TestPanel.cpp
#include "SurvivantEditor/Panels/BuildPanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantCore/Resources/ResourceRegistry.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace SvEditor::RuntimeBuild;

namespace SvEditor::Panels
{
    BuildPanel::BuildPanel() : 
        m_selectScene("Start Scene",
            SceneSelect::Ref([this]() -> SceneSelect::Value& { return m_scene; })),
        m_buildName("SvBuild"),
        m_selectBuildName("Build Name", m_buildName)
    {
        m_name = NAME;
        //m_scene = SvCore::Resources::GetDefaultResource<SvCore::ECS::Scene>();

        m_buttons.m_buttons.emplace_back(PanelButton("Build", [this]() 
            { 
                SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnCreateBuildGame>(
                    m_buildName, BuildConfig(m_scene));
            }));

        m_buttons.m_buttons.emplace_back(PanelButton("Build and Run", [this]()
            {
                //auto temp = SvEditor::RuntimeBuild::BuildManager::BuildConfigRef("", new BuildConfig(m_scene));
                //SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnCreateBuildAndRun>(
                //    m_buildName, temp);
            }));
    }

    BuildPanel::ERenderFlags BuildPanel::Render()
    {
        bool open = true;

        if (ImGui::Begin(m_name.c_str(), &open))
        {
            m_selectBuildName.DisplayAndUpdatePanel();
            m_buttons.DisplayAndUpdatePanel();
            m_selectScene.DisplayAndUpdatePanel();
        }

        ImGui::End();

        ERenderFlags flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }
}