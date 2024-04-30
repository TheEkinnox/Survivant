//PanelResourceSelector.inl
#pragma once

#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	template<class T>
	inline PanelResourceSelector<T>::PanelResourceSelector(
		const std::string& p_name, const Ref& p_resourceRef,
		const Callback& p_callback) :
		PanelInputBase<SvCore::Resources::ResourceRef<T>>(p_resourceRef, p_callback)
	{
		m_name = p_name;

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			"Swap",
			[this]() { this->GetAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));
	}

	template<class T>
	inline PanelResourceSelector<T>::PanelResourceSelector(const PanelResourceSelector& p_other) :
		PanelInputBase<SvCore::Resources::ResourceRef<T>>(p_other.m_getRef)
	{
		*this = p_other;
	}

	template<class T>
	inline PanelResourceSelector<T>::PanelResourceSelector(PanelResourceSelector&& p_other) :
		PanelInputBase<SvCore::Resources::ResourceRef<T>>(std::move(p_other.m_getRef))
	{
		this->m_getRef = std::move(p_other.m_getRef);
		this->m_name = std::move(p_other.m_name);

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			"Swap",
			[this]() { this->GetAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));
	}

	template<class T>
	inline PanelResourceSelector<T>& PanelResourceSelector<T>::operator=(const PanelResourceSelector& p_other)
	{
		this->m_getRef = p_other.m_getRef;
		this->m_name = p_other.m_name;

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			"Swap",
			[this]() { this->GetAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));
	}

	template<class T>
	inline void PanelResourceSelector<T>::DisplayAndUpdatePanel()
	{
		static auto flag = ImGuiInputTextFlags_ReadOnly;
		
		ImGui::Text(m_name.c_str());
		ImGui::SameLine();

		std::string resourceName = this->GetRef().GetPath();

		ImGui::PushID(resourceName.c_str());
		ImGui::InputText("##", &resourceName[0], resourceName.size(), flag);
		ImGui::PopID();

		m_allResources->DisplayAndUpdatePanel();
	}

	template<class T>
	inline void PanelResourceSelector<T>::GetAllResources()
	{
		using namespace SvCore::Resources;
		auto& rm = ResourceManager::GetInstance();

		std::vector<ResourceRef<T>> all = rm.GetAll<T>();

		m_allResources->m_items.clear();
		m_allResources->m_items.reserve(all.size());

		for (ResourceRef<T>& resource : all)
		{
			std::string name = resource.GetPath();

			m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(MenuButton(
				name, [this, resource](char) mutable {
					this->GetRef() = resource;
				}
			)));
		}
	}
}