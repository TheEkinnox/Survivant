//PanelResourceDisplay.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"
#include "SurvivantEditor/Interfaces/IInspectorable.h"
#include "SurvivantCore/Resources/ResourceRef.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelResourceDisplay : public Interfaces::IInspectorable
	{
	public:
		using Items = std::vector<std::shared_ptr<Interfaces::IPanelable>>;

		PanelResourceDisplay(
			const SvCore::Resources::GenericResourceRef& p_resource,
			const std::string& p_icon, 
			const Items& p_items);
		~PanelResourceDisplay() = default;

		// Inherited via IInspectorable
		void DisplayAndUpdatePanel() override;
		const std::string& GetIcon() override;
		const std::string& GetName() override;

		void SetItems(const Items& p_items);

	private:
		SvCore::Resources::GenericResourceRef m_resource;
		std::string		m_name;
		std::string		m_icon;
		Items			m_items;
	};
}