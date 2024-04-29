//PanelIntInput.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelIntInput : public PanelInputBase<int>
	{
	public:
		PanelIntInput(
			const std::string& p_name,
			int& p_value,
			const Callback& p_callback = Callback());
		PanelIntInput(
			const std::string& p_name,
			const GetRefFunc& p_refFunc,
			const Callback& p_callback = Callback());
		PanelIntInput(
			const std::string& p_name,
			const GetCopyFunc& p_copyFunc,
			const Callback& p_callback);
		~PanelIntInput() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string		m_name;
	};
}