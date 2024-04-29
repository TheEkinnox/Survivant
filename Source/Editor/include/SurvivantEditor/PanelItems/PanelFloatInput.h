//PanelFloatInput.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelFloatInput : public PanelInputBase<float>
	{
	public:
		using Callback = std::function<void(float)>;

		PanelFloatInput(
			const std::string& p_name,
			float& p_value,
			const Callback& p_callback = Callback());
		PanelFloatInput(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelFloatInput(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);
		~PanelFloatInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
	};
}