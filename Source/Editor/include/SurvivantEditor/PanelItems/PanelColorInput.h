//PanelColorInput.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include "Vector/Vector4.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelColorInput : public PanelInputBase<LibMath::Vector4>
	{
	public:
		PanelColorInput(
			const std::string& p_name,
			LibMath::Vector4& p_value,
			const Callback& p_callback = Callback());
		PanelColorInput(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelColorInput(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);
		~PanelColorInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string			m_name;
	};
}