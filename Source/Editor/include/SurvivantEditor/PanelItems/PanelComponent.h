//PanelComponent.h
#pragma once
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <memory>
#include <string>
#include <vector>

namespace SvEditor::PanelItems
{
	class PanelComponent : public Interfaces::IPanelable
	{
	public:
		using Items = std::vector<std::shared_ptr<IPanelable>>;

		PanelComponent(std::string p_name, Items p_items, size_t p_prio = 0);
		~PanelComponent() override = default;

		void		DisplayAndUpdatePanel() override;
		bool		NeedToRemove() const;
		size_t		GetPrio() const;

		const std::string&	GetName()const;

	protected:
		std::string m_name;
		Items		m_items;
		bool		m_isActive;

		size_t		m_prio;
	};
}

