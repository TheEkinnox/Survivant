//PanelFloatInput.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

#include <functional>

namespace SvEditor::UI::PanelItems
{
	class PanelFloatInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(float)>;

		PanelFloatInput(
			const std::string& p_name,
			float& p_value,
			const Callback& p_callback = Callback());
		~PanelFloatInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
		Callback		m_callback;
		float&			m_value;
	};
}