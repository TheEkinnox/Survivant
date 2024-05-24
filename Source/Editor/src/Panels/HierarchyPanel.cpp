//HierarchyPanel.cpp
#include "SurvivantEditor/Panels/HierarchyPanel.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/PanelItems/PanelEntity.h"
#include "SurvivantEditor/Panels/ExamplGameObj.h"
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"

#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantCore/Utility/Utility.h>

#include <imgui.h>

using namespace SvCore::ECS;
using namespace SvCore::Utility;

namespace SvEditor::Panels
{
    HierarchyPanel::HierarchyPanel() :
        m_tree("All", HierarchyBranch::Children(), false),
        m_addEntity( "Add Entity", []() {
        s_getCurrentScene().lock()->Get()->Create(); }),
        m_currentSelected(NULL_ENTITY)
    {
        m_name = NAME;
        SetScene();

        //propagate selection
        auto selectFunc = [this](HierarchyBranch& p_branch)
            {
                using namespace Core;

                auto entity = s_getCurrentScene().lock()->Get()->Find(p_branch.GetValue());
                auto entityPanel = InspectorItemManager::GetPanelableEntity(entity);

                m_currentSelected = entity.GetEntity();
                InspectorPanel::SetInpectorInfo(entityPanel, "Entity");
                ScenePanel::SelectEntity(entity);
                p_branch.ForceOpenParents();
                return false;
            };

        SvCore::Events::Event<>::EventDelegate clearFunc = [this]()
            {
                using namespace Core;

                m_currentSelected = NULL_ENTITY;
                auto entityPanel = InspectorItemManager::GetPanelableEntity(
                    EntityHandle());

                InspectorPanel::SetInpectorInfo(entityPanel, "Entity");
                ScenePanel::SelectEntity({});
            };

        HierarchyBranch::s_branchesOnSelect =   selectFunc;
        HierarchyBranch::s_leavesOnSelect =     selectFunc;
        HierarchyBranch::s_onClearSelected.AddListener(clearFunc);
    }

    HierarchyPanel::~HierarchyPanel()
    {
        if (m_scene.expired())
            return;

        RemoveListeners(GetScene());
    }

    void HierarchyPanel::RemoveListeners(SvCore::ECS::Scene& p_scene)
    {
        p_scene.GetStorage<Entity>().m_onAdd.RemoveListener(m_onModifEntity[0]);
        p_scene.GetStorage<Entity>().m_onRemove.RemoveListener(m_onModifEntity[1]);
        p_scene.GetStorage<SvCore::ECS::HierarchyComponent>().m_onAdd.RemoveListener(m_onModifHierarchy[0]);
        p_scene.GetStorage<SvCore::ECS::HierarchyComponent>().m_onRemove.RemoveListener(m_onModifHierarchy[1]);
        p_scene.GetStorage<SvCore::ECS::HierarchyComponent>().m_onChange.RemoveListener(m_onModifHierarchy[2]);
        p_scene.GetStorage<SvCore::ECS::TagComponent>().m_onAdd.RemoveListener(m_onModifTag[0]);
        p_scene.GetStorage<SvCore::ECS::TagComponent>().m_onRemove.RemoveListener(m_onModifTag[1]);
        p_scene.GetStorage<SvCore::ECS::TagComponent>().m_onChange.RemoveListener(m_onModifTag[2]);
    }

    void HierarchyPanel::SetScene()
    {
        ASSERT(s_getCurrentScene, "Current Scene Getter not set");

        auto oldSelection = m_currentSelected;
        auto oldScene = m_scene;
        m_scene = s_getCurrentScene();
        s_entities.clear();
        m_tree.SetBranches();
        SetupTree();

        if (oldSelection != NULL_ENTITY)
            ToggleSelectable(oldSelection.GetIndex());

        if (!oldScene.expired() && *m_scene.lock()->Get() == *oldScene.lock()->Get())
            return;

        if (!oldScene.expired())
            RemoveListeners(*oldScene.lock()->Get());

        //new scene
        SetSceneName();

        //tag dirty on hierarchy change
        m_onModifEntity[0] = GetScene().GetStorage<Entity>().m_onAdd.AddListener(
            [this](const EntityHandle& /*p_handle*/)
            {
                m_isDirty = true;
            });
        m_onModifEntity[1] = GetScene().GetStorage<Entity>().m_onRemove.AddListener(
            [this](const EntityHandle& /*p_handle*/)
            {
                m_isDirty = true;
            });

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

        auto modifTag = [](EntityHandle p_entity, SvCore::ECS::TagComponent /*p_tag*/)
            {
                auto it = s_entities.find(p_entity.GetEntity().GetIndex());
                [[unlikely]] if (it == s_entities.end())
                    return;

                auto& [index, branch] = *it;
                if (!branch.expired())
                    branch.lock()->SetName(p_entity.GetDisplayName());
            };
        m_onModifTag[0] = GetScene().GetStorage<SvCore::ECS::TagComponent>().m_onAdd.AddListener(modifTag);
        m_onModifTag[1] = GetScene().GetStorage<SvCore::ECS::TagComponent>().m_onRemove.AddListener(modifTag);
        m_onModifTag[2] = GetScene().GetStorage<SvCore::ECS::TagComponent>().m_onChange.AddListener(modifTag);
            
    }

    void HierarchyPanel::SetSceneName()
    {
        std::string path = m_scene.lock()->GetFullPath();
        m_sceneName = path.substr(path.find_last_of('\\') + 1);
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
        return std::make_shared<HierarchyBranch>(
            p_childEntity.GetDisplayName(), false, p_childEntity.GetEntity().GetIndex());
    }

    void HierarchyPanel::AddEntityBranch(
        HierarchyBranch& p_parent,
        std::shared_ptr<HierarchyBranch> p_childBranch)
    {
        auto prio = SIZE_MAX - p_parent.GetChildren().size();

        p_parent.AddBranch(p_childBranch, prio);
        s_entities.emplace(Entity::Index(p_childBranch->GetValue()), 
            std::weak_ptr<HierarchyBranch>(p_childBranch));
    }

    SvCore::ECS::Scene& HierarchyPanel::GetScene()
    {
        return *m_scene.lock()->Get();
    }

    Panel::ERenderFlags HierarchyPanel::Render()
    {
        bool open = true;
        auto windowFlags = ImGuiWindowFlags_NoNav | (dynamic_cast<Core::EditorEngine*>(SV_ENGINE())->IsEditorModifiedScene() ?
             ImGuiWindowFlags_UnsavedDocument : ImGuiWindowFlags_None);

        if (m_scene.expired() || m_isDirty)
        {
            if (!m_scene.expired())
                SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();

            SetScene();
            m_isDirty = false;
        }

        if (!ImGui::Begin(m_name.c_str(), &open, windowFlags))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        ImGui::Text(m_sceneName.c_str());

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

    void HierarchyPanel::ToggleSelectable(const SvCore::ECS::Entity::Index& p_entity)
    {
        auto it = s_entities.find(p_entity);

        if (it == s_entities.end())
        {
            auto currentSelected = SV_CURRENT_UI()->GetSelected();
            if (currentSelected != nullptr)
                currentSelected->ClearSelection();

            return;
        }

        auto& [id, branch] = *it;
        if (CHECK(!branch.expired(), "Can't ToggleSelectable, weakPtr expired"))
            branch.lock()->ToggleSelection();
    }

}
