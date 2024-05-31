//PanelEntitySelector.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"
#include "SurvivantEditor/PanelItems/PanelUInt32Input.h"

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Vector/Vector3.h"

#include <functional>
#include <string>

namespace SvEditor::PanelItems
{
	class PanelEntitySelector : public PanelInputBase<SvCore::ECS::EntityHandle>
	{
	public:
		PanelEntitySelector(
			const std::string& p_name,
			const GetRefFunc& p_resourceRef,
			const Callback& p_callback = Callback());
		PanelEntitySelector(
			const std::string& p_name,
			const GetCopyFunc& p_resourceCopy,
			const Callback& p_callback);
		~PanelEntitySelector() = default;

		PanelEntitySelector(const PanelEntitySelector& p_other);
		PanelEntitySelector(PanelEntitySelector&& p_other) noexcept;
		PanelEntitySelector& operator=(const PanelEntitySelector& p_other);

		virtual void DisplayAndUpdatePanel() override;

	private:
		PanelUInt32Input	m_input;
	};
}