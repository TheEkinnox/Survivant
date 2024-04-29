//PanelUInt32Input.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	class PanelUInt32Input : public PanelInputBase<uint32_t>
	{
	public:
		PanelUInt32Input(
			const std::string& p_name,
			Value& p_value,
			const Callback& p_callback = Callback());
		PanelUInt32Input(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelUInt32Input(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);
		~PanelUInt32Input() = default;

		virtual void DisplayAndUpdatePanel() override;

	private:
		std::string			m_name;
	};
}