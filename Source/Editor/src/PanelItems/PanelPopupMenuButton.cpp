//PanelPopupMenuButton.cpp

#include "SurvivantEditor/PanelItems/PanelPopupMenuButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	PanelPopupMenuButton::PanelPopupMenuButton(const std::string& p_name,
		Callback p_onOpen, Callback p_onClose) :
		m_name(p_name),
		m_onOpen(p_onOpen),
		m_onClose(p_onClose)
	{
		m_onOpen;
	}

	void PanelPopupMenuButton::DisplayAndUpdatePanel()
	{
		static auto flags = ImGuiPopupFlags_None;
		static bool isOpen = false;

		if (ImGui::SmallButton(m_name.c_str()))
		{
			ImGui::OpenPopup(ImGui::GetItemID());

			if (!isOpen && m_onOpen)
				m_onOpen();
		}

		if (ImGui::BeginPopupContextItem(nullptr, flags))
		{
			for (auto& menu : m_items)
				menu->DisplayAndUpdateMenu();

			ImGui::EndPopup();
			isOpen = true;
		}
		else
		{
			if (isOpen && m_onClose)
				m_onClose();

			isOpen = false;
		}

	}
}