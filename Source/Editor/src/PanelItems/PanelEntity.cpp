//PanelEntity.cpp
#include "SurvivantEditor/PanelItems/PanelEntity.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"


#include <SurvivantCore/ECS/ComponentRegistry.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>

#include <SurvivantScripting/LuaScriptList.h>

#include <imgui.h>

using namespace SvApp;

namespace SvEditor::PanelItems
{
    PanelEntity::PanelEntity(const SvCore::ECS::EntityHandle& p_entity, const Components& p_component) :
        m_entity(p_entity),
        m_luaScripts(std::make_unique<PanelScriptList>(p_entity))
    {
        m_index = "(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')';

        m_buttons.m_buttons.emplace_back("Add Child", [this]() { (void)m_entity.AddChild(); });
        m_buttons.m_buttons.emplace_back("Duplicate", [this]() { (void)m_entity.Copy(); });
        m_buttons.m_buttons.emplace_back("Remove", [this]() { m_entity.Destroy(); });

        m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component",
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        m_components.reserve(p_component.size());
        for (auto& component : p_component)
            AddAndSortComponent(component);
    }

    PanelEntity::PanelEntity(const PanelEntity& p_other)
    {
        *this = p_other;
    }

    PanelEntity::PanelEntity(PanelEntity&& p_other) noexcept
    {
        *this = std::move(p_other);
    }

    PanelEntity& PanelEntity::operator=(const PanelEntity& p_other)
    {
        if (&p_other == this)
            return *this;

        m_buttons.m_buttons.emplace_back("Add Child", [this]() { (void)m_entity.AddChild(); });
        m_buttons.m_buttons.emplace_back("Duplicate", [this]() { (void)m_entity.Copy(); });
        m_buttons.m_buttons.emplace_back("Remove", [this]() { m_entity.Destroy(); });

        this->m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component",
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        m_luaScripts = std::make_unique<PanelScriptList>(p_other.m_entity);

        this->m_components = p_other.m_components;
        this->m_entity = p_other.m_entity;
        this->m_index = p_other.m_index;
        this->m_name = p_other.m_name;

        return *this;
    }

    PanelEntity& PanelEntity::operator=(PanelEntity&& p_other) noexcept
    {
        if (&p_other == this)
            return *this;

        m_buttons.m_buttons.emplace_back("Add Child", [this]() { (void)m_entity.AddChild(); });
        m_buttons.m_buttons.emplace_back("Duplicate", [this]() { (void)m_entity.Copy(); });
        m_buttons.m_buttons.emplace_back("Remove", [this]() { m_entity.Destroy(); });

        this->m_addComponent = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
            "Add Component",
            [this]() { GetAllComponents(); },
            [this]() { this->m_addComponent->m_items.clear(); }
        ));

        this->m_luaScripts = std::move(p_other.m_luaScripts);
        this->m_components = std::move(p_other.m_components);
        this->m_entity = std::move(p_other.m_entity);
        this->m_index = std::move(p_other.m_index);
        this->m_name = std::move(p_other.m_name);

        return *this;
    }

    void PanelEntity::DisplayAndUpdatePanel()
	{
        if (!m_entity)
            return;

        m_buttons.DisplayAndUpdatePanel();

        if (!m_entity) //can remove entity in m_buttons
            return;

        auto& style = ImGui::GetStyle();
        auto prev = style.WindowPadding.x;
        style.WindowPadding.x = 30;

        ImGui::SeparatorText("Components");

        for (auto it = m_components.begin(); it != m_components.end();)
        {
            if (it != m_components.begin())
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            (*it)->DisplayAndUpdatePanel();

            if ((*it)->NeedToRemove())
            {
                SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
                it = RemoveComponent(it);
            }
            else
                ++it;
        }

        m_luaScripts->DisplayList();
        style.WindowPadding.x = prev;

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        m_addComponent->DisplayAndUpdatePanel();

        ImGui::SameLine();

        m_luaScripts->DisplayAddScript();
	}

    const std::string& PanelEntity::GetIcon()
    {
        return LOGO;
    }

    const std::string& PanelEntity::GetName()
    {
        using namespace SvCore::ECS;
        static std::string name;

        name = m_entity.GetDisplayName();
        return name;
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

        auto& compReg = ComponentRegistry::GetInstance();
        m_addComponent->m_items.clear();

        std::set<std::string> currentComponents;
        for (auto& comp : m_components)
            currentComponents.emplace(std::string(comp->GetName()));

        for (auto& name : compReg.GetRegisteredNames())
        {
            // Components can be added only once and scripts are handled by the entity panel
            if (currentComponents.contains(name) || name == compReg.GetRegisteredTypeName<SvScripting::LuaScriptList>())
                continue;

            m_addComponent->m_items.emplace_back(std::make_shared<MenuButton>(MenuButton(
                name, [this, type = compReg.GetTypeInfo(name)](char) {
                    AddAndSortComponent(InspectorItemManager::AddPanelableComponent(type, m_entity));
                    SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
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
