//PanelVec2Input.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include "Vector/Vector2.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelVec2Input : public PanelInputBase<LibMath::Vector2>
	{
	public:
		PanelVec2Input(
			const std::string& p_name,
			LibMath::Vector2& p_value,
			const Callback& p_callback = Callback());
		PanelVec2Input(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelVec2Input(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);
		~PanelVec2Input() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string			m_name;
	};
}