//TestPanel.cpp
#include "SurvivantEditor/Panels/BuildPanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Events/EventManager.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


namespace SvEditor::Panels
{
    BuildPanel::BuildPanel() : 
        m_addScene("Add Scene", 
            SceneSelect::Ref([this]() -> SceneSelect::Value& { static SceneSelect::Value val; return val; }), 
            false, 
            [this](SceneSelect::CallbackParams p_scene) { AddScene(p_scene); }),
        m_removeScene("Remove Scene", 
            SceneSelect::Ref([this]() -> SceneSelect::Value& { static SceneSelect::Value val; return val; }),
            false, 
            [this](SceneSelect::CallbackParams p_scene) { RemoveScene(p_scene); }),
        m_buildName(),
        m_selectBuildName("Build Name", m_buildName)
    {
        m_name = NAME;

        m_buttons.m_buttons.emplace_back(PanelButton("Build", []() 
            { 
                ASSERT(false, "Build Event Not Linked");
                //SV_EVENT_MANAGER().Invoke<>(); 
            }));

        m_buttons.m_buttons.emplace_back(PanelButton("Build and Run", []()
            {
                //ASSERT(false, "Build and Run Events Not Linked");
                SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnRunBuildGame>(std::string("notepad.exe"));
            }));
    }

    BuildPanel::ERenderFlags BuildPanel::Render()
    {
        bool open = true;

        if (ImGui::Begin(m_name.c_str(), &open))
        {
            m_selectBuildName.DisplayAndUpdatePanel();
            m_buttons.DisplayAndUpdatePanel();

            m_addScene.DisplayAndUpdatePanel();
            ImGui::SameLine();
            m_removeScene.DisplayAndUpdatePanel();

            m_scenePaths.DisplayAndUpdatePanel();
        }

        ImGui::End();

        ERenderFlags flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags(flags | ERenderFlags::CLOSE);

        return flags;
    }

    void BuildPanel::AddScene(const SceneSelect::Value& p_scene)
    {
        RemoveScene(p_scene);

        m_scenePaths.AddItem(std::make_shared<DefaultText>(DefaultText(p_scene.GetPath())));
        m_scenes.emplace_back(SceneSelect::Value(p_scene));
    }

    void BuildPanel::RemoveScene(const SceneSelect::Value& p_scene)
    {
        auto index = m_scenePaths.Contains(p_scene.GetPath());
        if (index == -1)
            return;

        m_scenePaths.Remove(index);
        m_scenes.erase(m_scenes.begin() + index);
    }
}