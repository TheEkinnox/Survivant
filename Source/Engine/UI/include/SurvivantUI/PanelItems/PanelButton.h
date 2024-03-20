//PanelButton.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelButton : public Interfaces::IPanelable
	{
	public:
		PanelButton(const std::string& p_name, const std::function<void()>& p_callback = std::function<void()>()) :
			m_name(p_name), m_callback(p_callback)
		{}
		~PanelButton() = default;

		void DisplayAndUpdatePanel()override;

		void AddCallback(const std::function<void()>& p_callback)
		{
			m_callback = p_callback;
		}

	private:
		std::string	m_name;
		std::function<void()> m_callback;
	};
}