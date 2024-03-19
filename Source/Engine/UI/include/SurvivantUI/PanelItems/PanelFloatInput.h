//PanelFloatInput.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelFloatInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(float)>;

		PanelFloatInput(
			const std::string& p_name,
			const float& p_value,
			const Callback& p_callback);
		~PanelFloatInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
		Callback		m_callback;
		float			m_value;
	};
}