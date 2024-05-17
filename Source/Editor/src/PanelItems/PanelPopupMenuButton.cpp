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

	/*PanelPopupMenuButton::PanelPopupMenuButton(const PanelPopupMenuButton& p_other)
	{
		*this = p_other;
	}

	PanelPopupMenuButton::PanelPopupMenuButton(PanelPopupMenuButton&& p_other) noexcept
	{
		this->m_onOpen = std::move(p_other.m_onOpen);
		this->m_onClose = std::move(p_other.m_onClose);
		this->m_name = std::move(p_other.m_name);
		this->m_items = std::move(p_other.m_items);
	}

	PanelPopupMenuButton& PanelPopupMenuButton::operator=(const PanelPopupMenuButton& p_other)
	{
		this->m_name = p_other.m_name;
		this->m_onOpen = p_other.m_onOpen;
		this->m_onClose = p_other.m_onClose;

		this->m_items.clear();
		this->m_items.reserve(p_other.m_items.size());
		for (const auto& item : p_other.m_items)
		{
			this->m_items.emplace_back();
			this->m_items.back().reset(item->Clone());
		}

		this->m_items.shrink_to_fit();

		return *this;
	}

	PanelPopupMenuButton* PanelPopupMenuButton::Clone() const
	{
		return new PanelPopupMenuButton(*this);
	}*/

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