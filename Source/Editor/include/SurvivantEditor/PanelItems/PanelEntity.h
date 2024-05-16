//PanelEntity.h
#pragma once

#include "SurvivantEditor/Interfaces/IInspectorable.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelPopupMenuButton.h"
#include "SurvivantEditor/PanelItems/PanelScriptList.h"

#include <SurvivantCore/ECS/EntityHandle.h>

#include <vector>

namespace SvEditor::PanelItems
{
	class PanelEntity : public Interfaces::IInspectorable
	{
	public:
		using Components = std::vector<std::shared_ptr<PanelComponent>>;

		PanelEntity(const SvCore::ECS::EntityHandle& p_entity, const Components& p_component);
		~PanelEntity() override = default;

		PanelEntity(const PanelEntity& p_other);
		PanelEntity(PanelEntity&& p_other) noexcept;
		PanelEntity& operator=(const PanelEntity& p_other);
		PanelEntity& operator=(PanelEntity&& p_other) noexcept;

		// Inherited via IPanelable
		virtual void DisplayAndUpdatePanel() override;

		// Inherited via IInspectorable
		const std::string& GetIcon() override;
		const std::string& GetName() override;

	private:
		static inline const std::string LOGO = "En";

		void AddAndSortComponent(std::shared_ptr<PanelComponent> p_component);

		void GetAllComponents();

		Components::iterator RemoveComponent(const Components::iterator& p_it);

		std::string					m_index;
		std::string					m_name;
		SvCore::ECS::EntityHandle	m_entity;
		Components					m_components;
		PanelButton					m_remove;
		PanelButton					m_addChild;
		PanelButton					m_duplicate;

		std::shared_ptr<PanelPopupMenuButton>	m_addComponent;
		std::unique_ptr<PanelScriptList>		m_luaScripts;
	};
}
