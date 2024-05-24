//PanelVec3Input.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelVec3Input : public PanelInputBase<LibMath::Vector3>
	{
	public:
		PanelVec3Input(
			const std::string& p_name,
			LibMath::Vector3& p_value,
			const Callback& p_callback = Callback(),
			float p_min = -FLT_MAX,
			float p_max = FLT_MAX);
		PanelVec3Input(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback(),
			float p_min = -FLT_MAX,
			float p_max = FLT_MAX);
		PanelVec3Input(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback,
			float p_min = -FLT_MAX,
			float p_max = FLT_MAX);
		~PanelVec3Input() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string	m_name;
		float		m_min;
		float		m_max;
	};
}