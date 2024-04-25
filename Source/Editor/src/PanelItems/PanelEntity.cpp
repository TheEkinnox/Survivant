//PanelEntity.cpp

#include "SurvivantEditor/PanelItems/PanelEntity.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantEditor/Core/IUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelEntity::PanelEntity(const SvCore::ECS::EntityHandle& p_entity, const Components& p_component) :
        m_entity(p_entity)
    {
        m_index = "(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')';

        m_components.reserve(p_component.size());
        for (auto& component : p_component)
            AddAndSortComponent(component);
    }

    void PanelEntity::DisplayAndUpdatePanel()
	{
        if (!m_entity)
        {
            m_entity = SvCore::ECS::EntityHandle();
            m_components.clear();
        }

        for (auto it = m_components.begin(); it != m_components.end();)
        {
            ImGui::Separator();

            it->DisplayAndUpdatePanel();

            if (it->NeedToRemove())
                it = m_components.erase(it);
            else
                ++it;
        }
        ImGui::Separator();
        
        //m_addComponentButton.DisplayAndUpdatePanel();
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

    void PanelEntity::AddAndSortComponent(const PanelComponent& p_component)
    {
        static auto prioFunc = [](const PanelComponent& p_a, const PanelComponent& p_b)
            {
                return p_a.GetPrio() > p_b.GetPrio();
            };

        auto first = m_components.begin();
        auto last = m_components.end();
        auto newPrio = p_component.GetPrio();

        if (first == last || newPrio == 0)
            m_components.push_back(p_component);

        else if (first->GetPrio() < newPrio)
            m_components.insert(first, p_component);

        else
            m_components.insert(
                std::upper_bound(m_components.begin(), m_components.end(), p_component, prioFunc),
                p_component);
    }
}