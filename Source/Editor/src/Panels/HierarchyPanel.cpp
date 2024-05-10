//HierarchyPanel.cpp

#include "SurvivantEditor/Panels/HierarchyPanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantCore/Utility/Utility.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/PanelItems/PanelEntity.h"
#include "SurvivantEditor/Panels/ExamplGameObj.h"
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace SvCore::ECS;
using namespace SvCore::Utility;

namespace SvEditor::Panels
{
    HierarchyPanel::HierarchyPanel() :
        m_tree("All", HierarchyBranch::Children(), false),
        m_addEntity( "Add Entity", []() {
        s_getCurrentScene().lock()->Get()->Create(); })
    {
        m_name = NAME;

        UpdateScene();

        //propagate selection
        auto selectFunc = [](HierarchyBranch& p_branch)
            { 
                using namespace Core;

                auto val = p_branch.GetValue();
                auto entityPanel = InspectorItemManager::GetPanelableEntity(
                    EntityHandle(s_getCurrentScene().lock()->Get(), Entity(val)));

                InspectorPanel::SetInpectorInfo(entityPanel, "Entity");
                ScenePanel::SelectEntity(val);
                p_branch.ForceOpenParents();
                return false; 
            };

        HierarchyBranch::s_branchesOnSelect =   selectFunc;
        HierarchyBranch::s_leavesOnSelect =     selectFunc;
    }

    HierarchyPanel::~HierarchyPanel()
    {
        if (m_scene.expired())
            return;

        GetScene().GetStorage<Entity>().m_onAdd.RemoveListener(     m_onModifEntity[0]);
        GetScene().GetStorage<Entity>().m_onRemove.RemoveListener(  m_onModifEntity[1]);
        GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onAdd.RemoveListener(    m_onModifHierarchy[0]);
        GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onRemove.RemoveListener( m_onModifHierarchy[1]);
        GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onChange.RemoveListener( m_onModifHierarchy[2]);
    }

    void HierarchyPanel::UpdateScene()
    {
        ASSERT(s_getCurrentScene, "Current Scene Getter not set");

        auto oldScene = m_scene;
        m_scene = s_getCurrentScene();
        m_tree.SetBranches();
        SetupTree();

        if (!oldScene.expired() && *m_scene.lock()->Get() == *oldScene.lock()->Get())
            return;

        m_onModifEntity[0] = GetScene().GetStorage<Entity>().m_onAdd.AddListener(
            [this](const EntityHandle& /*p_handle*/)
            {
                //auto parent = p_handle.GetParent();
                //auto& parentBranch = parent.GetEntity() == NULL_ENTITY ? m_tree : *s_entities.at(p_handle.GetEntity());

                //auto childBranch = CreateEntityBranch(p_handle);
                //AddEntityBranch(parentBranch, childBranch);
                m_isDirty = true;
            });
        m_onModifEntity[1] = GetScene().GetStorage<Entity>().m_onRemove.AddListener(
            [this](const EntityHandle& /*p_handle*/)
            {
                m_isDirty = true;
                //RemoveEntity(p_handle);
            });

        //tag dirty on hierarchy change
        m_onModifHierarchy[0] = GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onAdd.AddListener(
            [this](EntityHandle, SvCore::ECS::HierarchyComponent)
            {
                m_isDirty = true;
            });
        m_onModifHierarchy[1] = GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onRemove.AddListener(
            [this](EntityHandle, SvCore::ECS::HierarchyComponent)
            {
                m_isDirty = true;
            });
        m_onModifHierarchy[2] = GetScene().GetStorage<SvCore::ECS::HierarchyComponent>().m_onChange.AddListener(
            [this](EntityHandle, SvCore::ECS::HierarchyComponent)
            {
                m_isDirty = true;
            }); 

        if (!oldScene.expired())
        {
            auto& oldRef = *oldScene.lock()->Get();
            oldRef.GetStorage<Entity>().m_onAdd.RemoveListener(m_onModifEntity[0]);
            oldRef.GetStorage<Entity>().m_onRemove.RemoveListener(m_onModifEntity[1]);
            oldRef.GetStorage<SvCore::ECS::HierarchyComponent>().m_onAdd.RemoveListener(m_onModifHierarchy[0]);
            oldRef.GetStorage<SvCore::ECS::HierarchyComponent>().m_onRemove.RemoveListener(m_onModifHierarchy[1]);
            oldRef.GetStorage<SvCore::ECS::HierarchyComponent>().m_onChange.RemoveListener(m_onModifHierarchy[2]);
        }
    }

    void HierarchyPanel::SetupTree()
    {
        auto& entities = GetScene().GetStorage<Entity>();

        for (auto& entity : entities)
        {
            auto handle = EntityHandle(&GetScene(), entity);

            if (handle.GetParent().GetEntity() != NULL_ENTITY)
                continue;

            auto newBranch = CreateEntityBranch(handle);
            AddEntityBranch(m_tree, newBranch);
            SetupEntityBranch(*newBranch, handle);
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
            name, false, p_childEntity.GetEntity());
    }

    void HierarchyPanel::AddEntityBranch(
        HierarchyBranch& p_parent, 
        std::shared_ptr<HierarchyBranch> p_childBranch)
    {
        auto prio = SIZE_MAX - p_parent.GetChildren().size();

        p_parent.AddBranch(p_childBranch, prio);
        /*auto ins = */s_entities.insert({ p_childBranch->GetValue(), p_childBranch}).second;

        //ASSERT(ins, "Entity cant be added");
    }

    void HierarchyPanel::RemoveEntity(const SvCore::ECS::EntityHandle& p_entity)
    {
        auto it = s_entities.find(p_entity.GetEntity());

        if (it == s_entities.end())
            return;

        auto& childBranch = it->second;

        std::set<SvCore::ECS::Entity::Id> deleted;
        childBranch->DeleteBranch(&deleted);

        //unless parent is already removed
        m_tree.RemoveBranch(childBranch->GetName());

        for (auto& item : deleted)
            s_entities.erase(item);
    }

    /*void HierarchyPanel::SetupChildsEntityValue(const HierarchyBranch::Children& p_childreen)
    {
        if (p_childreen.size() == 0)
            return;

        for (auto& [name, branch] : p_childreen)
        {

            s_entities.emplace(branch->GetValue(), branch);
        }

        for (auto& [name, branch] : p_childreen)
            SetupChildsEntityValue(branch->GetChildren());
    }*/

    SvCore::ECS::Scene& HierarchyPanel::GetScene()
    {
        return *m_scene.lock()->Get();
    }

    Panel::ERenderFlags HierarchyPanel::Render()
    {
        bool open = true;

        if (m_scene.expired() || m_isDirty)
        {
            UpdateScene();
            m_isDirty = false;
        }

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        for (auto& branch : m_tree.GetChildren())
            branch.second->DisplayAndUpdatePanel();

        m_addEntity.DisplayAndUpdatePanel();

        ImGui::End();

        auto flags = Panel::ERenderFlags();
        if (!open)
            flags = Panel::ERenderFlags::CLOSE;

        return flags;
    }

    void HierarchyPanel::SetCurrentSceneGetter(const CurrentSceneGetter& p_getCurrentScene)
    {
        s_getCurrentScene = p_getCurrentScene;
    }

    void HierarchyPanel::SelectSelectable(const SvCore::ECS::Entity::Id& p_entity)
    {
        auto it = s_entities.find(p_entity);

        if (it == s_entities.end())
        {
            auto currentSelected = SV_CURRENT_UI()->GetSelected();
            if (currentSelected != nullptr)
                currentSelected->ClearSelection();

            return;
        }

        it->second->Select();
    }

}
