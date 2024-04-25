//PanelComponent.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"


#include <functional>
#include <memory>
#include <string>
#include <type_traits>

namespace SvEditor::PanelItems
{
	class PanelComponent : Interfaces::IPanelable
	{
	public:
		using Items = std::vector<std::shared_ptr<Interfaces::IPanelable>>;

		PanelComponent(const std::string& p_name, const Items& p_items, size_t p_prio = 0);
		~PanelComponent() = default;

        void	DisplayAndUpdatePanel() override;
		bool	NeedToRemove()const;
		size_t	GetPrio()const;

	private:
		std::string m_name;
		Items		m_items;
		bool		m_isActive;

		size_t		m_prio;
	};
}

