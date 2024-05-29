#include "SurvivantEditor/PanelItems/PanelGroup.h"

#include <imgui.h>

using namespace LibMath;

namespace SvEditor::PanelItems
{
    PanelGroup::PanelGroup(Items p_items, GroupSettings p_settings)
        : m_items(std::move(p_items)), m_settings(std::move(p_settings)), m_defaultOpen(!m_settings.m_isCollapsed)
    {
    }

    void PanelGroup::DisplayAndUpdatePanel()
    {
        const char* label       = !m_settings.m_label.empty() ? m_settings.m_label.c_str() : nullptr;
        const bool  canCollapse = m_settings.m_isCollapsible && label;

        ImGui::PushID(this);

        if (canCollapse)
        {
            static constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog;
            m_settings.m_isCollapsed = !ImGui::TreeNodeEx(label, flags | (m_defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0));
        }
        else if (label)
        {
            ImGui::SeparatorText(label);
        }

        if (!canCollapse || !m_settings.m_isCollapsed)
        {
            if (m_settings.m_offset.magnitudeSquared() > 0.f)
            {
                ImGui::Dummy(reinterpret_cast<const ImVec2&>(m_settings.m_offset));
                ImGui::SameLine();
            }

            ImGui::BeginGroup();

            for (const auto& item : m_items)
                item->DisplayAndUpdatePanel();

            ImGui::EndGroup();
        }

        ImGui::PopID();
    }
}
