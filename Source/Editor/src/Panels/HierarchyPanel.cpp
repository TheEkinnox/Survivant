//HierarchyPanel.cpp

#include "SurvivantEditor/Panels/HierarchyPanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantCore/Utility/Utility.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Panels/ExamplGameObj.h"
#include "SurvivantEditor/Panels/ScenePanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace SvCore::ECS;
using namespace SvCore::Utility;

namespace SvEditor::Panels
{
    HierarchyPanel::HierarchyPanel() :
        m_tree("All", HierarchyBranch::Childreen(), false)
    {
        m_name = NAME;

        UpdateScene();
        GetScene().GetStorage<Entity>().m_onAdd.AddListener(
            [this](const EntityHandle& p_handle)
            {
                auto parent = p_handle.GetParent();
                auto& parentBranch = parent.GetEntity() == NULL_ENTITY ? m_tree : *s_entities.at(p_handle.GetEntity());

                auto childBranch = CreateEntityBranch(p_handle);
                AddEntityBranch(parentBranch, childBranch);
            });
        GetScene().GetStorage<Entity>().m_onRemove.AddListener(
            [this](const EntityHandle& p_handle)
            {
                RemoveEntity(p_handle);
            });

        //propagate selection
        auto selectFunc = [](HierarchyBranch& p_branch)
            { 
                ScenePanel::s_selectedEntity = p_branch.GetValue();
                return false; 
            };

        HierarchyBranch::s_branchesOnSelect =   selectFunc;
        HierarchyBranch::s_leavesOnSelect =     selectFunc;
    }

    HierarchyPanel::~HierarchyPanel()
    {
    }

    void HierarchyPanel::UpdateScene()
    {
        ASSERT(s_getCurrentScene, "Current Scene Getter not set");
        m_scene = s_getCurrentScene();

        m_tree.SetBranches();

        SetupTree();
    }

    void HierarchyPanel::SetupTree()
    {
        auto& entities = GetScene().GetStorage<Entity>();

        for (auto& entity : entities)
        {
            auto handle = EntityHandle(&GetScene(), entity);

            if (handle.GetParent().GetEntity() != NULL_ENTITY)
                continue;

            AddEntityBranch(m_tree, CreateEntityBranch(handle));
            SetupEntityBranch(m_tree, handle);
        }
    }

    void HierarchyPanel::SetupEntityBranch(
        HierarchyBranch& p_parent, const SvCore::ECS::EntityHandle& p_entity)
    {
        using WithChildreen = std::vector<std::pair< std::shared_ptr<HierarchyBranch>, SvCore::ECS::EntityHandle>>;
        WithChildreen withChildreen;

        for (auto& child: p_entity.GetChildren())
        {
            auto branch = CreateEntityBranch(child);
            AddEntityBranch(p_parent, branch);

            if (child.GetChildCount() != 0)
                withChildreen.push_back({ branch, child });
        }

        for (auto& [parent, entity] : withChildreen)
            SetupEntityBranch(*parent, entity);
    }

    std::shared_ptr<HierarchyPanel::HierarchyBranch> HierarchyPanel::CreateEntityBranch(
        const SvCore::ECS::EntityHandle& p_childEntity)
    {
        auto name = FormatString("Entity(%d)", p_childEntity.GetEntity().GetIndex());

        return std::make_shared<HierarchyBranch>(
            name, false, p_childEntity.GetEntity().GetIndex());
    }

    void HierarchyPanel::AddEntityBranch(
        HierarchyBranch& p_parent, 
        std::shared_ptr<HierarchyBranch> p_childBranch)
    {
        auto prio = SIZE_MAX - p_parent.GetChildreen().size();

        p_parent.AddBranch(p_childBranch, prio);
        auto ins = s_entities.insert({ p_childBranch->GetValue(), p_childBranch}).second;

        ASSERT(ins, "Entity cant be added");
    }

    void HierarchyPanel::RemoveEntity(const SvCore::ECS::EntityHandle& p_entity)
    {
        auto it = s_entities.find(p_entity.GetEntity());

        if (it == s_entities.end())
            return;

        auto& childBranch = it->second;

        //always have parrent bcs tree is root
        childBranch->GetParent()->RemoveBranch(childBranch->GetName());

        auto val = childBranch.use_count();
        ASSERT(val == 1, "HELP HERE");
    }

    /*void HierarchyPanel::SetupChildsEntityValue(const HierarchyBranch::Childreen& p_childreen)
    {
        if (p_childreen.size() == 0)
            return;

        for (auto& [name, branch] : p_childreen)
        {

            s_entities.emplace(branch->GetValue(), branch);
        }

        for (auto& [name, branch] : p_childreen)
            SetupChildsEntityValue(branch->GetChildreen());
    }*/

    SvCore::ECS::Scene& HierarchyPanel::GetScene()
    {
        return *m_scene.lock().get()->get();
    }

    Panel::ERenderFlags HierarchyPanel::Render()
    {
        bool open = true;

        if (m_scene.expired())
            UpdateScene();

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

    void HierarchyPanel::SetCurrentSceneGetter(CurrentSceneGetter p_getCurrentScene)
    {
        s_getCurrentScene = p_getCurrentScene;
    }

    void HierarchyPanel::SelectSelectable(const SvCore::ECS::Entity::Id& p_entity)
    {
        auto it = s_entities.find(p_entity);
        ASSERT(it != s_entities.end(), "Entity cant be selected | Not in hierarchy");

        it->second->InvokeSelected();
    }

}
