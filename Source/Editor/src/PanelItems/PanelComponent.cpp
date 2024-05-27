//PanelComponent.cpp

#include "SurvivantEditor/PanelItems/PanelComponent.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelComponent::PanelComponent(const std::string& p_name, const Items& p_items, size_t p_prio) :
        m_name(p_name),
        m_items(p_items),
        m_isActive(true),
        m_prio(p_prio)
    {}

    void PanelComponent::DisplayAndUpdatePanel()
	{
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

        bool open = ImGui::CollapsingHeader(m_name.c_str(), &m_isActive, flags);
        ImGui::PushID(m_name.c_str());

        ImGui::Spacing();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
        ImGui::BeginGroup();
        if (open)
        {
            for (auto& item : m_items)
                item->DisplayAndUpdatePanel();
        }
        ImGui::EndGroup();

        ImGui::PopID();
	}

    bool PanelComponent::NeedToRemove()const
    {
        return !m_isActive;
    }

    size_t PanelComponent::GetPrio()const
    {
        return m_prio;
    }
    const std::string& PanelComponent::GetName() const
    {
        return m_name;
    }
}