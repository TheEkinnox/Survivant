//PopupMenu.cpp

#include "SurvivantUI/MenuItems/PopupMenu.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//#include "imgui_internal.h"

namespace SvUI::MenuItems
{
	PopupMenu::PopupMenu(const PopupMenu& p_other)
	{
		*this = p_other;
	}

	PopupMenu::PopupMenu(PopupMenu&& p_other) noexcept
	{
		this->m_items = std::move(p_other.m_items);
	}

	PopupMenu& PopupMenu::operator=(const PopupMenu& p_other)
	{
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

	PopupMenu* PopupMenu::Clone() const
	{
		return new PopupMenu(*this);
	}

	void PopupMenu::DisplayAndUpdatePanel()
	{
		static auto flags = ImGuiPopupFlags_MouseButtonRight;

		if (IsEmpty())
			return;

		//ImGui::PushID();
		if (ImGui::BeginPopupContextItem(NULL, flags))
		{
			for (auto& menu : m_items)
				menu->DisplayAndUpdateMenu();

			ImGui::EndPopup();
		}

	}
}