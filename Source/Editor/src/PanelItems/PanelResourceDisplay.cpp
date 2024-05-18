//PanelResourceDisplay.cpp
#include "SurvivantEditor/PanelItems/PanelResourceDisplay.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	SvEditor::PanelItems::PanelResourceDisplay::PanelResourceDisplay(
		const SvCore::Resources::GenericResourceRef& p_resource,
		const std::string& p_icon,
		const Items& p_items) :
		m_resource(p_resource),
		m_icon(p_icon),
		m_items(p_items)
	{
		m_name = m_resource.GetType();
	}

	void SvEditor::PanelItems::PanelResourceDisplay::DisplayAndUpdatePanel()
	{
		ImGui::Text(m_resource.GetPath().c_str());
		ImGui::Separator();

		for (auto& item : m_items)
			item->DisplayAndUpdatePanel();
	}

	const std::string& PanelResourceDisplay::GetIcon()
	{
		return m_icon;
	}

	const std::string& PanelResourceDisplay::GetName()
	{
		return m_name;
	}

	void PanelResourceDisplay::SetItems(const Items& p_items)
	{
		m_items = p_items;
	}
}


