//PanelEntity.h
#pragma once

#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Interfaces/IInspectorable.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"

#include "Vector/Vector3.h"

#include <functional>
#include <vector>

namespace SvEditor::PanelItems
{
	class PanelEntity : public Interfaces::IInspectorable
	{
	public:
		using Components = std::vector<PanelComponent>;

		PanelEntity(const SvCore::ECS::EntityHandle& p_entity, const Components& p_component);
		~PanelEntity() = default;

		// Inherited via IPanelable
		virtual void DisplayAndUpdatePanel() override;

		// Inherited via IInspectorable
		const std::string& GetIcon() override;
		const std::string& GetName() override;

	private:
		static inline const std::string LOGO = "En";

		void AddAndSortComponent(const PanelComponent& p_component);


		std::string					m_index;
		std::string					m_name;
		SvCore::ECS::EntityHandle	m_entity;
		Components					m_components;
		//PanelButton					m_addComponentButton;
	};
}