//PanelVec3Input.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::UI::PanelItems
{
	class PanelVec3Input : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(LibMath::Vector3)>;

		PanelVec3Input(
			const std::string& p_name,
			LibMath::Vector3& p_value,
			const Callback& p_callback);
		~PanelVec3Input() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string			m_name;
		Callback			m_callback;
		LibMath::Vector3&	m_value;

	};
}