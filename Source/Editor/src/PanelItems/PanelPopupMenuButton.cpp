//PanelPopupMenuButton.cpp

#include "SurvivantEditor/PanelItems/PanelPopupMenuButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	PanelPopupMenuButton::PanelPopupMenuButton(const std::string& p_name, const Callback& p_callback) :
		m_name(p_name),
		m_callback(p_callback)
	{}

	PanelPopupMenuButton::PanelPopupMenuButton(const PanelPopupMenuButton& p_other)
	{
		*this = p_other;
	}

	PanelPopupMenuButton::PanelPopupMenuButton(PanelPopupMenuButton&& p_other) noexcept
	{
		this->m_name = std::move(p_other.m_name);
		this->m_items = std::move(p_other.m_items);
	}

	PanelPopupMenuButton& PanelPopupMenuButton::operator=(const PanelPopupMenuButton& p_other)
	{
		this->m_name = p_other.m_name;

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
	}

	void PanelPopupMenuButton::DisplayAndUpdatePanel()
	{
		static auto flags = ImGuiPopupFlags_None;

		if (IsEmpty())
			return;

		if (ImGui::SmallButton(m_name.c_str()))
		{
			ImGui::OpenPopup(ImGui::GetItemID());

			if (m_callback)
				m_callback();
		}

		if (ImGui::BeginPopupContextItem(nullptr, flags))
		{
			for (auto& menu : m_items)
				menu->DisplayAndUpdateMenu();

			ImGui::EndPopup();
		}

	}
}