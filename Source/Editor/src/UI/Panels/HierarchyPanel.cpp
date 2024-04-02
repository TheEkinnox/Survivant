//HierarchyPanel.cpp

#include "SurvivantEditor/UI/Panels/HierarchyPanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/UI/Core/EditorUI.h"

#include "SurvivantEditor/UI/Panels/ExamplGameObj.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


namespace SvEditor::UI::Panels
{
    void HierarchyPanel::SetupTree()
    {
        std::vector<std::shared_ptr<PanelTreeBranch>> objects;
        std::string name = "Obj ";

        for (size_t i = 0; i < 20; i++)
        {
            auto ref = objects.insert(objects.end(), std::make_shared<PanelTreeBranch>(name + std::to_string(i), false));

            if (i % 3)
            {
                ref->get()->SetBranches(
                    { 
                        std::make_shared<PanelTreeBranch>(name + std::to_string(i) + ".1", false),
                        std::make_shared<PanelTreeBranch>(name + std::to_string(i) + ".2", false),
                        std::make_shared<PanelTreeBranch>(name + std::to_string(i) + ".3", false),
                    });
            }
        }

        m_tree.SetBranches(objects);
    }

    HierarchyPanel::HierarchyPanel() :
        m_tree("All", false)
    {
        m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());

        SetupTree();
    }

    HierarchyPanel::~HierarchyPanel()
    {
    }

    Panel::ERenderFlags HierarchyPanel::Render()
    {
        bool open = true;

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        for (auto& branch : m_tree.GetChildreen())
            branch.second->DisplayAndUpdatePanel();


        ImGui::End();

        auto flags = Panel::ERenderFlags();
        if (!open)
            flags = Panel::ERenderFlags::CLOSE;

        return flags;
    }

}
