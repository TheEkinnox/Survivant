//PanelResourceSelector.inl
#pragma once

#include "SurvivantEditor/MenuItems/MenuButton.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"

#include <SurvivantCore/Resources/ResourceRegistry.h>
#include <SurvivantCore/Resources/ResourceManager.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	template<class T, class RefT>
	PanelResourceSelector<T, RefT>::PanelResourceSelector(
		const std::string& p_name, const Ref& p_resourceRef,
		const Callback& p_callback) :
		PanelResourceSelector(p_name, p_resourceRef, true, p_callback)
	{}

	template<class T, class RefT>
	PanelResourceSelector<T, RefT>::PanelResourceSelector(
		const std::string& p_name, const Ref& p_resourceRef, bool p_displayResource, const Callback& p_callback) :
		PanelInputBase<RefT>(p_resourceRef, p_callback)
	{
		m_name = p_name;
		m_displayResource = p_displayResource;

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			m_name,
			[this]() { this->FetchAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));
	}

	template<class T, class RefT>
	PanelResourceSelector<T, RefT>::PanelResourceSelector(const PanelResourceSelector& p_other) :
		PanelInputBase<RefT>({})
	{
		*this = p_other;
	}

	template<class T, class RefT>
	PanelResourceSelector<T, RefT>::PanelResourceSelector(PanelResourceSelector&& p_other) noexcept :
		PanelInputBase<RefT>({})
	{
		*this = std::move(p_other);
	}

	template<class T, class RefT>
	PanelResourceSelector<T, RefT>& PanelResourceSelector<T, RefT>::operator=(const PanelResourceSelector& p_other)
	{
		if (this == &p_other)
			return *this;

		this->m_getRef = p_other.m_getRef;
		this->m_callback = p_other.m_callback;
		this->m_name = p_other.m_name;
		this->m_displayResource = p_other.m_displayResource;

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			m_name,
			[this]() { this->FetchAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));

		return *this;
	}

	template <class T, class RefT>
	PanelResourceSelector<T, RefT>& PanelResourceSelector<T, RefT>::operator=(PanelResourceSelector&& p_other) noexcept
	{
		if (this == &p_other)
			return *this;

		this->m_getRef = std::move(p_other.m_getRef);
		this->m_callback = std::move(p_other.m_callback);
		this->m_name = std::move(p_other.m_name);
		this->m_displayResource = std::move(p_other.m_displayResource);

		m_allResources = std::make_shared<PanelPopupMenuButton>(PanelPopupMenuButton(
			m_name,
			[this]() { this->FetchAllResources(); },
			[this]() { this->m_allResources->m_items.clear(); }
		));

		return *this;
	}

	template<class T, class RefT>
	void PanelResourceSelector<T, RefT>::DisplayAndUpdatePanel()
	{
		static auto flag = ImGuiInputTextFlags_ReadOnly;

		m_allResources->DisplayAndUpdatePanel();

		if (m_displayResource)
		{
			ImGui::SameLine();
			std::string resourceName = this->GetRef().GetPath();
			resourceName = resourceName.empty() ? "(none)" : resourceName;
			ImGui::PushID(resourceName.c_str());
			ImGui::InputText("##", &resourceName[0], resourceName.size(), flag);
			ImGui::PopID();
		}
	}

	template<class T, class RefT>
	void PanelResourceSelector<T, RefT>::FetchAllResources()
	{
		using namespace SvCore::Resources;
		using namespace SvEditor::Panels;

		auto& rr = ResourceRegistry::GetInstance();

		auto all = ContentDrawerPanel::GetAllFilePaths(rr.GetRegisteredTypeName<T>());

		m_allResources->m_items.clear();
		m_allResources->m_items.reserve(all.size() + 1);

		m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(MenuButton(
			"(default)", [this](char) mutable {
				this->GetRef() = {};
				if (this->m_callback)
					this->m_callback({});
			}
		)));

		if constexpr (!std::is_abstract_v<T>)
		{
			for (const std::string& resourcePath : all)
			{
				m_allResources->m_items.emplace_back(std::make_unique<MenuButton>(MenuButton(
					resourcePath, [this, resourcePath](char) mutable {
						RefT resource = ResourceManager::GetInstance().Load<T>(resourcePath);
						this->GetRef() = resource;

						if (this->m_callback)
							this->m_callback(resource);
					}
				)));
			}
		}
	}
}