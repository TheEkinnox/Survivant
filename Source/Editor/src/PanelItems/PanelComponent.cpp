//PanelComponent.cpp
#include "SurvivantEditor/PanelItems/PanelComponent.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    PanelComponent::PanelComponent(std::string p_name, Items p_items, const size_t p_prio)
        : m_name(std::move(p_name)), m_items(std::move(p_items)), m_isActive(true), m_prio(p_prio)
    {
    }

    void PanelComponent::DisplayAndUpdatePanel()
    {
        ImGuiTreeNodeFlags flags = GetPrio() != 0?  ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;

        if (!ImGui::CollapsingHeader(m_name.c_str(), &m_isActive, flags))
            return;

        ImGui::PushID(m_name.c_str());
        ImGui::Spacing();

        ImGui::Dummy(ImVec2{ X_OFFSET, 0.f });
        ImGui::SameLine();

        ImGui::BeginGroup();

        for (const auto& item : m_items)
            item->DisplayAndUpdatePanel();

        ImGui::EndGroup();
        ImGui::PopID();
    }

    bool PanelComponent::NeedToRemove() const
    {
        return !m_isActive;
    }

    size_t PanelComponent::GetPrio() const
    {
        return m_prio;
    }

    const std::string& PanelComponent::GetName() const
    {
        return m_name;
    }
}
