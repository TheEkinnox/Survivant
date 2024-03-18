//PanelIntInput.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelIntInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(int)>;

		PanelIntInput(
			const std::string& p_name,
			int p_value,
			const Callback& p_callback);
		~PanelIntInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
		Callback		m_callback;
		int				m_value;
	};
}