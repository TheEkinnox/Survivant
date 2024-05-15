//PanelEntity.cpp
#include "SurvivantEditor/PanelItems/PanelEntity.h"

#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelEntity::PanelEntity(const SvCore::ECS::EntityHandle& p_entity, const Components& p_component) :
        m_entity(p_entity)
    {
        m_index = "(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')';
        m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component", 
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        m_components.reserve(p_component.size());
        for (auto& component : p_component)
            AddAndSortComponent(component);

        m_buttons.m_buttons.emplace_back("Duplicate", [e = m_entity]() { e.GetScene()->Create(e); });
        m_buttons.m_buttons.emplace_back("Remove", [e = m_entity]() { e.GetScene()->Destroy(e); });
    }

    PanelEntity::PanelEntity(const PanelEntity& p_other)
    {
        *this = p_other;
    }

    PanelEntity::PanelEntity(PanelEntity&& p_other) noexcept 
    {
        m_buttons.m_buttons.emplace_back("Duplicate", [e = p_other.m_entity]() { e.GetScene()->Create(e); });
        m_buttons.m_buttons.emplace_back("Remove", [e = p_other.m_entity]() { e.GetScene()->Destroy(e); });
        this->m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component",
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        this->m_components = std::move(p_other.m_components);
        this->m_entity = std::move(p_other.m_entity);
        this->m_index = std::move(p_other.m_index);
        this->m_name = std::move(p_other.m_name);
    }

    PanelEntity& PanelEntity::operator=(const PanelEntity& p_other)
    {
        m_buttons.m_buttons.emplace_back("Duplicate", [e = p_other.m_entity]() { e.GetScene()->Create(e); });
        m_buttons.m_buttons.emplace_back("Remove", [e = p_other.m_entity]() { e.GetScene()->Destroy(e); });
        this->m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component",
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        this->m_components = p_other.m_components;
        this->m_entity = p_other.m_entity;
        this->m_index = p_other.m_index;
        this->m_name = p_other.m_name;

        return *this;
    }

    void PanelEntity::DisplayAndUpdatePanel()
	{
        if (!m_entity)
            return;

        m_buttons.DisplayAndUpdatePanel();

        if (!m_entity) //can remove entity in m_buttons
            return;

        for (auto it = m_components.begin(); it != m_components.end();)
        {
            ImGui::Separator();

            (*it)->DisplayAndUpdatePanel();

            if ((*it)->NeedToRemove())
                it = RemoveComponent(it);
            else
                ++it;
        }
        ImGui::Separator();
        m_addComponent->DisplayAndUpdatePanel();
	}

    const std::string& PanelEntity::GetIcon()
    {
        return LOGO;
    }

    const std::string& PanelEntity::GetName()
    {
        using namespace SvCore::ECS;

        auto val = m_entity.Get<TagComponent>();
        if (val)
            return m_name = val->m_tag + m_index;

        return m_index;
    }

    void PanelEntity::AddAndSortComponent(std::shared_ptr<PanelComponent> p_component)
    {
        static auto prioFunc = 
            [](const std::shared_ptr<PanelComponent>& p_a, const std::shared_ptr<PanelComponent>& p_b)
            {
                return p_a->GetPrio() > p_b->GetPrio();
            };

        auto first = m_components.begin();
        auto last = m_components.end();
        auto newPrio = p_component->GetPrio();

        if (first == last || newPrio == 0)
            m_components.push_back(p_component);

        else if ((*first)->GetPrio() < newPrio)
            m_components.insert(first, p_component);

        else
            m_components.insert(
                std::upper_bound(m_components.begin(), m_components.end(), p_component, prioFunc),
                p_component);
    }

    void PanelEntity::GetAllComponents()
    {
        using namespace SvCore::ECS;
        using namespace SvEditor::Core;

        auto& rm = ComponentRegistry::GetInstance();
        m_addComponent->m_items.clear();

        std::set<std::string> currentComponents;
        for (auto& comp : m_components)
            currentComponents.emplace(std::string(comp->GetName()));

        for (auto& name : rm.GetRegisteredNames())
        {
            if (currentComponents.contains(name))
                continue;
            
            m_addComponent->m_items.emplace_back(std::make_shared<MenuButton>(MenuButton(
                name, [this, type = rm.GetTypeInfo(name)](char) {
                    AddAndSortComponent(InspectorItemManager::AddPanelableComponent(type, m_entity));}
            )));            
        }
    }

    PanelEntity::Components::iterator PanelEntity::RemoveComponent(
        const Components::iterator& p_it)
    {
        using namespace SvCore::ECS;

        auto& typeInfo = ComponentRegistry::GetInstance().GetTypeInfo(p_it->get()->GetName());

        m_entity.GetScene()->GetStorage(typeInfo.m_typeId).Remove(m_entity);
        //m_entity.Remove(typeInfo.m_typeId);

        return m_components.erase(p_it);
    }
}